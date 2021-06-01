#!/usr/bin/env python3

import os
import argparse
import sys
import time
import subprocess
import psutil
import signal
import select
import multiprocessing
from multiprocessing.managers import BaseManager
from multiprocessing import Lock, Process, Value
import queue
import threading
import contextlib
import glob
import shutil
import copy
import serial
import yaml
import json
import string
import logging


logger = logging.getLogger('osp-test')
logger.setLevel(logging.DEBUG)

EMBARC_ROOT = os.getenv("EMBARC_ROOT")
if not EMBARC_ROOT:
    logger.error('EMBARC_ROOT unset')
    sys.exit(0)

MAKEFILENAMES = ['Makefile', 'makefile', 'GNUMakefile']


def find_executable(executable, path=None):
    _, ext = os.path.splitext(executable)
    if (sys.platform == 'win32') and (ext != '.exe'):
        executable = executable + '.exe'

    if os.path.isfile(executable):
        return executable

    if path is None:
        path = os.environ.get('PATH', None)
        if path is None:
            try:
                path = os.confstr("CS_PATH")
            except (AttributeError, ValueError):
                path = os.defpath
    if not path:
        return None

    paths = path.split(os.pathsep)
    for p in paths:
        f = os.path.join(p, executable)
        if os.path.isfile(f):
            return f
    return None


def parse_arguments():
    parser = argparse.ArgumentParser()
    parser.add_argument("-b", "--build-only", action="store_true",
                        help="Only build the code, do not execute any of it")
    parser.add_argument("-T", "--testcase-root", action="append", default=[],
                        help="Base directory to search for test cases.")
    parser.add_argument("-p", "--platform", action="append",
                        help="Platform filter for testing"
                        "Testcases will only be built/run on the platforms ")
    parser.add_argument("--platform-version", action="append",
                        help="Platform version filter for testing")
    parser.add_argument("--core", action="append",
                        help="Platform core filter for testing")
    parser.add_argument("--toolchain", choices=["gnu", "mw"], default="gnu",
                        help="Toolchain filter for testing")
    parser.add_argument("--device-testing", action="store_true",
                        help="Test on device directly. Specify the serial device"
                        "to use with the --device-serial option.")
    parser.add_argument("--device-serial",
                        help="""Serial device for accessing the board
                        (e.g., /dev/ttyACM0)
                        """)
    parser.add_argument('--digilent-device', default="",
                        help="if given, selects FTDI instance by its"
                        "digilent number(toolchain is gnu) or digilent"
                        "name(toolchain is mw), defaults to empty")
    parser.add_argument("-j", "--jobs", type=int,
                        help="Number of jobs for building, defaults to number"
                        "of CPU threads, overcommited by factor 2 when --build-only")
    parser.add_argument("-O", "--outdir", default=os.path.join(os.getcwd(), "test-out"),
                        help="Output directory for logs and binaries. ")
    parser.add_argument("-o", "--report-dir",
                        help="Output report into the specified directory.")
    parser.add_argument("-v", "--verbose", action="count", default=0,
                        help="Emit debugging information, call "
                        "multiple times to increase verbosity")
    return parser.parse_args()


class ExecutionCounter(object):
    def __init__(self, total=0):
        self._done = Value('i', 0)
        self._passed = Value('i', 0)
        self._skipped_configs = Value('i', 0)
        self._skipped_runtime = Value('i', 0)
        self._skipped_cases = Value('i', 0)
        self._error = Value('i', 0)
        self._failed = Value('i', 0)
        self._total = Value('i', total)
        self._cases = Value('i', 0)

        self.lock = Lock()

    @property
    def cases(self):
        with self._cases.get_lock():
            return self._cases.value

    @cases.setter
    def cases(self, value):
        with self._cases.get_lock():
            self._cases.value = value

    @property
    def skipped_cases(self):
        with self._skipped_cases.get_lock():
            return self._skipped_cases.value

    @skipped_cases.setter
    def skipped_cases(self, value):
        with self._skipped_cases.get_lock():
            self._skipped_cases.value = value

    @property
    def error(self):
        with self._error.get_lock():
            return self._error.value

    @error.setter
    def error(self, value):
        with self._error.get_lock():
            self._error.value = value

    @property
    def done(self):
        with self._done.get_lock():
            return self._done.value

    @done.setter
    def done(self, value):
        with self._done.get_lock():
            self._done.value = value

    @property
    def passed(self):
        with self._passed.get_lock():
            return self._passed.value

    @passed.setter
    def passed(self, value):
        with self._passed.get_lock():
            self._passed.value = value

    @property
    def skipped_configs(self):
        with self._skipped_configs.get_lock():
            return self._skipped_configs.value

    @skipped_configs.setter
    def skipped_configs(self, value):
        with self._skipped_configs.get_lock():
            self._skipped_configs.value = value

    @property
    def skipped_runtime(self):
        with self._skipped_runtime.get_lock():
            return self._skipped_runtime.value

    @skipped_runtime.setter
    def skipped_runtime(self, value):
        with self._skipped_runtime.get_lock():
            self._skipped_runtime.value = value

    @property
    def failed(self):
        with self._failed.get_lock():
            return self._failed.value

    @failed.setter
    def failed(self, value):
        with self._failed.get_lock():
            self._failed.value = value

    @property
    def total(self):
        with self._total.get_lock():
            return self._total.value


def is_embarc_osp_makefile(makefile):
    with open(makefile) as f:
        embarc_root = False
        appl = False
        lines = f.read().splitlines()
        for line in lines:
            if "EMBARC_ROOT" in line:
                embarc_root = True
            if "APPL" in line:
                appl = True
            if embarc_root and appl:
                return True
        return False


class DUT(object):
    def __init__(self,
                 id=None,
                 serial=None,
                 platform=None,
                 connected=False,
                 pre_script=None,
                 post_script=None,
                 post_flash_script=None):

        self.serial = serial
        self.platform = platform
        self._counter = Value("i", 0)
        self._available = Value("i", 1)
        self.connected = connected
        self.pre_script = pre_script
        self.id = id
        self.post_flash_script = post_flash_script
        self.post_script = post_script
        self.pre_script = pre_script
        self.lock = Lock()

    @property
    def available(self):
        with self._available.get_lock():
            return self._available.value

    @available.setter
    def available(self, value):
        with self._available.get_lock():
            self._available.value = value

    @property
    def counter(self):
        with self._counter.get_lock():
            return self._counter.value

    @counter.setter
    def counter(self, value):
        with self._counter.get_lock():
            self._counter.value = value

    def to_dict(self):
        d = {}
        exclude = ['_available', '_counter', 'match']
        v = vars(self)
        for k in v.keys():
            if k not in exclude and v[k]:
                d[k] = v[k]
        return d

    def __repr__(self):
        return f"<{self.platform} ({self.product}) on {self.serial}>"


class HardwareMap:

    def __init__(self):
        self.detected = []
        self.duts = []

    def add_device(self, serial, platform, pre_script):
        device = DUT(platform=platform, connected=True, pre_script=pre_script)
        device.serial = serial

        self.duts.append(device)


class Platform:
    def __init__(self, name, version=None, core=None):
        """Constructor.

        """
        self.name = name
        self.version = version
        self.core = core
        self.run = False

        self.type = "na"
        self.simulation = "na"

    def load(self, platform_file):
        pass

    def get_versions(self, example):
        result = list()
        cmd = ["make", "EMBARC_ROOT=%s" % EMBARC_ROOT]
        cmd.append("BOARD=%s" % (self.name))
        cmd.extend(["-C", example])
        cmd.append("spopt")
        try:
            output = subprocess.check_output(cmd, stderr=subprocess.STDOUT)
            if output:
                opt_lines = output.decode("utf-8").splitlines()
                for opt_line in opt_lines:
                    if opt_line.startswith("SUPPORTED_BD_VERS"):
                        platform_versions = opt_line.split(":", 1)[1]
                        result.extend(platform_versions.split())
                        break
        except subprocess.CalledProcessError as ex:
            logger.error("Fail to run command {}".format(cmd))
            sys.exit(ex.output.decode("utf-8"))
        return result

    def get_cores(self, example, version):
        result = list()
        cmd = ["make", "EMBARC_ROOT=%s" % EMBARC_ROOT]
        cmd.append("BOARD=%s" % (self.name))
        cmd.append("BD_VER=%s" % (version))
        cmd.extend(["-C", example])
        cmd.append("spopt")
        try:
            output = subprocess.check_output(cmd, stderr=subprocess.STDOUT)
            if output:
                opt_lines = output.decode("utf-8").splitlines()
                for opt_line in opt_lines:
                    if opt_line.startswith("SUPPORTED_CORES"):
                        platforms_cores = opt_line.split(":", 1)[1]
                        result.extend(platforms_cores.split())
                        break
        except subprocess.CalledProcessError as ex:
            logger.error("Fail to run command {}".format(cmd))
            sys.exit(ex.output.decode("utf-8"))
        return result

    def __repr__(self):
        return "<%s version %s core %s on arc>" % (self.name, self.version, self.core)


class DisablePyTestCollectionMixin(object):
    __test__ = False


class TestCase(DisablePyTestCollectionMixin):
    """Class representing a test application
    """

    def __init__(self, testcase_root, workdir, name):
        self.source_dir = ""

        self.name = name
        self.id = name
        self.platform_exclude = None
        self.platform_allow = None
        self.toolchain_exclude = None
        self.toolchain_allow = None
        self.tc_filter = None
        self.build_only = False
        self.timeout = 300
        self.cases = []
        self.tags = set()
        self.skip = False
        self.extra_args = None

    def __str__(self):
        return self.name


class TestInstance(DisablePyTestCollectionMixin):
    def __init__(self, testcase, platform, toolchain, outdir):

        self.testcase = testcase
        self.platform = platform
        self.toolchain = toolchain

        self.status = None
        self.reason = "Unknown"
        self.metrics = dict()
        self.handler = None
        self.outdir = outdir

        self.name = os.path.join(platform.name, platform.version, platform.core, testcase.name)
        self.build_dir = os.path.join(outdir, platform.name, platform.version, platform.core, testcase.name)

        self.run = False

        self.results = {}

    def __getstate__(self):
        d = self.__dict__.copy()
        return d

    def __setstate__(self, d):
        self.__dict__.update(d)

    def __lt__(self, other):
        return self.name < other.name

    def fill_results_by_status(self):
        status_to_verdict = {
            'skipped': 'SKIP',
            'error': 'BLOCK',
            'failure': 'FAILED'
        }

        for k in self.results:
            self.results[k] = status_to_verdict[self.status]

    def __repr__(self):
        return "<TestCase %s on %s %s %s>" % (self.testcase.name, self.platform.name, self.platform.version, self.platform.core)


class Handler:
    def __init__(self, instance, type_str="build"):
        """Constructor

        """
        self.state = "waiting"
        self.run = False
        self.duration = 0
        self.type_str = type_str
        self.terminated = False

        self.binary = None
        self.pid_fn = None
        self.call_make_run = False

        self.name = instance.name
        self.instance = instance
        self.timeout = instance.testcase.timeout
        self.sourcedir = instance.testcase.source_dir
        self.build_dir = instance.build_dir
        self.log = os.path.join(self.build_dir, "handler.log")
        self.returncode = 0
        self.set_state("running", self.duration)
        self.generator = None
        self.generator_cmd = None

        self.args = []

    def set_state(self, state, duration):
        self.state = state
        self.duration = duration

    def get_state(self):
        ret = (self.state, self.duration)
        return ret

    def try_kill_process_by_pid(self):
        if self.pid_fn:
            pid = int(open(self.pid_fn).read())
            os.unlink(self.pid_fn)
            self.pid_fn = None
            try:
                os.kill(pid, signal.SIGTERM)
            except ProcessLookupError:
                pass

    def terminate(self, proc):
        for child in psutil.Process(proc.pid).children(recursive=True):
            try:
                os.kill(child.pid, signal.SIGTERM)
            except ProcessLookupError:
                pass
        proc.terminate()
        time.sleep(0.5)
        proc.kill()
        self.terminated = True


class Harness:
    CONSOLE_START = "Compiler Version:"
    CONSOLE_END = "Test Done"
    BLINKY_MESSAGE = "led out:"
    SKIPPED_MESSAGE = "This example is not supported"
    EXCEPTION_INFO = [
        "Memory Error",
        "Instruction Error",
        "EV_MachineCheck: ",
        "EV_TLBMissI",
        "EV_TLBMissD",
        "EV_ProtV: ",
        "EV_PrivilegeV: ",
        "EV_SWI",
        "EV_Trap",
        "EV_Extension",
        "EV_DivZero",
        "EV_DCError",
        "EV_Misaligned",
        "EV_VecUnit",
        "unknown exception vector"
    ]

    def __init__(self, instance):
        self.state = None
        self.capture_console = False
        self.simulator_blinky = 0
        self.id = instance.testcase.id
        self.tests = {}

    def handle(self, line):
        for ex in self.EXCEPTION_INFO:
            if ex in line:
                self.state = "failed"
        if self.CONSOLE_START in line:
            self.capture_console = True
        if self.CONSOLE_END in line:
            if self.capture_console:
                self.state = "passed"
            self.capture_console = False
        if self.SKIPPED_MESSAGE in line:
            self.state = "skipped"
        if self.BLINKY_MESSAGE in line:
            if self.capture_console and not self.simulator_blinky:
                self.simulator_blinky = 1
            if self.simulator_blinky:
                self.simulator_blinky += 1
                if self.simulator_blinky > 4:
                    self.state = "passed"
        if self.state == "passed":
            self.tests[self.id] = "PASS"
        else:
            self.tests[self.id] = "FAIL"


class BinaryHandler(Handler):
    def __init__(self, instance, type_str):
        """Constructor

        @param instance Test Instance
        """
        super().__init__(instance, type_str)

        # Tool options
        self.coverage = False

    def _output_reader(self, proc, harness):
        log_out_fp = open(self.log, "wt")
        for line in iter(proc.stdout.readline, b''):
            line = line.decode('utf-8').rstrip()
            logger.debug("OUTPUT: {0}".format(line))
            log_out_fp.write(line + "\n")
            log_out_fp.flush()
            harness.handle(line)
            if harness.state:
                try:
                    proc.wait(0.1)
                except subprocess.TimeoutExpired:
                    self.terminate(proc)
                break

        log_out_fp.close()

    def handle(self):
        harness = Harness(self.instance)
        command = ["make", "BOARD=%s" % (self.instance.platform.name),
                   "BD_VER=%s" % (self.instance.platform.version),
                   "CUR_CORE=%s" % (self.instance.platform.core),
                   "TOOLCHAIN=%s" % (self.instance.toolchain),
                   "EMBARC_ROOT=%s" % EMBARC_ROOT,
                   "-C", os.path.join(EMBARC_ROOT, self.instance.testcase.source_dir),
                   "run"]
        start_time = time.time()
        env = os.environ.copy()

        with subprocess.Popen(command, stdout=subprocess.PIPE,
                              stderr=subprocess.PIPE, cwd=self.instance.build_dir, env=env) as proc:
            logger.debug("Spawning BinaryHandler Thread for %s" % self.name)
            t = threading.Thread(target=self._output_reader, args=(proc, harness), daemon=True)
            t.start()
            t.join(self.timeout)
            if t.is_alive():
                self.terminate(proc)
                t.join()
            proc.wait()
            self.returncode = proc.returncode
            self.try_kill_process_by_pid()

        handler_time = time.time() - start_time

        if sys.stdout.isatty():
            subprocess.call(["stty", "sane"])

        self.instance.results = harness.tests

        if not self.terminated and self.returncode != 0:
            self.set_state("failed", handler_time)
            self.instance.reason = "Failed"
        elif harness.state:
            self.set_state(harness.state, handler_time)
            if harness.state == "failed":
                self.instance.reason = "Failed"
        else:
            self.set_state("timeout", handler_time)
            self.instance.reason = "Timeout"


class DeviceHandler(Handler):
    def __init__(self, instance, type_str):
        """Constructor

        @param instance Test Instance
        """
        super().__init__(instance, type_str)

        self.suite = None

    def device_is_available(self, instance):
        device = instance.platform.name
        for d in self.suite.duts:
            if d.platform != device or not d.serial:
                continue
            d.lock.acquire()
            avail = False
            if d.available:
                d.available = 0
                d.counter += 1
                avail = True
            d.lock.release()
            if avail:
                return d

        return None

    def make_device_available(self, serial):
        for d in self.suite.duts:
            if d.serial == serial:
                d.available = 1

    def monitor_serial(self, ser, halt_fileno, harness):
        log_out_fp = open(self.log, "wt")

        ser_fileno = ser.fileno()
        readlist = [halt_fileno, ser_fileno]

        ser.flush()

        while ser.isOpen():
            readable, _, _ = select.select(readlist, [], [], self.timeout)

            if halt_fileno in readable:
                logger.warning('halted')
                ser.close()
                break
            if ser_fileno not in readable:
                continue

            serial_line = None
            try:
                serial_line = ser.readline()
            except TypeError:
                pass
            except serial.SerialException:
                ser.close()
                break

            if serial_line:
                sl = serial_line.decode('utf-8', 'ignore').lstrip()
                logger.debug("DEVICE: {0}".format(sl.rstrip()))

                log_out_fp.write(sl)
                log_out_fp.flush()
                harness.handle(sl.rstrip())

            if harness.state:
                ser.close()
                break

        log_out_fp.close()

    def handle(self):
        out_state = "failed"
        harness = Harness(self.instance)
        digilent_parameter = "%s=%s" % (
            "DIG_NUM" if self.instance.toolchain == "gnu" else "DIG_NAME",
            self.instance.platform.digilent_device
        )

        command = ["make", "BOARD=%s" % (self.instance.platform.name),
                   "BD_VER=%s" % (self.instance.platform.version),
                   "CUR_CORE=%s" % (self.instance.platform.core),
                   "TOOLCHAIN=%s" % (self.instance.toolchain),
                   "EMBARC_ROOT=%s" % EMBARC_ROOT,
                   digilent_parameter,
                   "-C", os.path.join(EMBARC_ROOT, self.instance.testcase.source_dir),
                   "run"]

        hardware = self.device_is_available(self.instance)
        while not hardware:
            logger.debug("Waiting for device {} to become available".format(self.instance.platform.name))
            time.sleep(1)
            hardware = self.device_is_available(self.instance)

        serial_device = hardware.serial

        logger.debug("Using serial device {}".format(serial_device))

        try:
            ser = serial.Serial(
                serial_device,
                baudrate=115200,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                bytesize=serial.EIGHTBITS,
                timeout=self.timeout
            )
        except serial.SerialException as e:
            self.set_state("failed", 0)
            self.instance.reason = "Failed"
            logger.warning("Serial device error: %s" % (str(e)))

            self.make_device_available(serial_device)
            return

        ser.flush()
        read_pipe, write_pipe = os.pipe()
        start_time = time.time()

        t = threading.Thread(target=self.monitor_serial, daemon=True,
                             args=(ser, read_pipe, harness))
        t.start()

        d_log = "{}/device.log".format(self.instance.build_dir)
        logger.info('Flash command: %s', command)
        try:
            stdout = stderr = None
            with subprocess.Popen(command, stderr=subprocess.PIPE, stdout=subprocess.PIPE, 
                                  cwd=self.instance.build_dir, env=os.environ.copy()) as proc:
                try:
                    (stdout, stderr) = proc.communicate(timeout=30)
                    if proc.returncode != 0:
                        self.instance.reason = "Device issue (Flash?)"
                        with open(d_log, "w") as dlog_fp:
                            dlog_fp.write(stderr.decode())
                        os.write(write_pipe, b'x')
                        out_state = "flash_error"
                except subprocess.TimeoutExpired:
                    self.terminate(proc)
                    self.instance.reason = "Device issue (Timeout)"

            with open(d_log, "w") as dlog_fp:
                if stderr:
                    dlog_fp.write(stderr.decode())

        except subprocess.CalledProcessError:
            os.write(write_pipe, b'x')

        t.join(self.timeout)
        if t.is_alive():
            logger.debug("Timed out while monitoring serial output on {}".format(self.instance.platform.name))
            out_state = "timeout"

        if ser.isOpen():
            ser.close()

        os.close(write_pipe)
        os.close(read_pipe)

        handler_time = time.time() - start_time

        if out_state in ["timeout", "flash_error"]:
            for c in self.instance.testcase.cases:
                if c not in harness.tests:
                    harness.tests[c] = "BLOCK"

            if out_state == "timeout":
                self.instance.reason = "Timeout"
            elif out_state == "flash_error":
                self.instance.reason = "Flash error"

        self.instance.results = harness.tests

        if self.instance.results == {}:
            for k in self.instance.testcase.cases:
                self.instance.results[k] = 'BLOCK'

        if harness.state:
            self.set_state(harness.state, handler_time)
            if harness.state == "failed":
                self.instance.reason = "Failed"
        else:
            self.set_state(out_state, handler_time)

        self.make_device_available(serial_device)


class ProjectBuilder:
    def __init__(self, suite, instance, **kwargs):
        self.log = "build.log"
        self.instance = instance
        self.suite = suite
        self.source_dir = instance.testcase.source_dir
        self.build_dir = instance.build_dir
        self.platform = instance.platform
        self.testcase = instance.testcase

        self.device_testing = kwargs.get('device_testing', False)

    def setup_handler(self):
        instance = self.instance

        if instance.platform.simulation == "nsim":
            if find_executable("nsimdrv"):
                instance.handler = BinaryHandler(instance, "nsim")
                instance.handler.call_make_run = True
        elif self.device_testing:
            instance.handler = DeviceHandler(instance, "device")

    def process(self, pipeline, done, message, lock, results):
        op = message.get('op')

        if not self.instance.handler:
            self.setup_handler()

        if op == "build":
            res = self.build()

            if not res:
                self.instance.status = "error"
                self.instance.reason = "Build Failure"
                pipeline.put({"op": "report", "test": self.instance})
            else:
                inst = res.get("instance", None)
                if inst and inst.status == "skipped":
                    results.skipped_runtime += 1

                if res.get('returncode', 1) > 0:
                    pipeline.put({"op": "report", "test": self.instance})
                else:
                    if self.instance.run and self.instance.handler:
                        pipeline.put({"op": "run", "test": self.instance})
                    else:
                        pipeline.put({"op": "report", "test": self.instance})
        elif op == "run":
            self.run()
            self.instance.status, _ = self.instance.handler.get_state()
            logger.debug(f"run status: {self.platform.name} {self.platform.version} {self.platform.core} {self.instance.name} {self.instance.status}")

            self.instance.handler.thread = None
            self.instance.handler.suite = None
            pipeline.put({
                "op": "report",
                "test": self.instance,
                "status": self.instance.status,
                "reason": self.instance.reason
                }
            )
        elif op == "report":
            with lock:
                done.put(self.instance)
                self.report_out(results)

    def build(self, **kwargs):
        kwargs['stdout'] = subprocess.PIPE
        kwargs['stderr'] = subprocess.STDOUT

        cmd = ["make", "BOARD=%s" % (self.platform.name),
               "BD_VER=%s" % (self.platform.version),
               "CUR_CORE=%s" % (self.platform.core),
               "TOOLCHAIN=%s" % (self.instance.toolchain),
               "-C", os.path.join(EMBARC_ROOT, self.source_dir),
               "OUT_DIR_ROOT=%s" % (self.build_dir),
               "EMBARC_ROOT=%s" % EMBARC_ROOT]
        if self.instance.testcase.extra_args:
            cmd.extend(self.instance.testcase.extra_args.split())

        env = os.environ.copy()

        p = subprocess.Popen(cmd, cwd=self.build_dir, env=env, **kwargs)
        out, _ = p.communicate()
        if out:
            with open(os.path.join(self.build_dir, self.log), "a") as log:
                log.write(out.decode())
        results = {}
        msg = "build status: %s version %s core %s %s %s" % (
            self.platform.name,
            self.platform.version,
            self.platform.core,
            self.instance.name,
            "failed" if p.returncode else "passed"
        )
        logger.debug(msg)
        if p.returncode == 0:
            self.instance.status = "passed"
            results = {'msg': msg, "returncode": p.returncode, "instance": self.instance}
            if not out:
                return None
        else:
            self.instance.status = "error"
            self.instance.reason = "Build failure"

            results = {
                "returncode": p.returncode,
                "instance": self.instance,
            }
        sys.stdout.flush()
        return results

    def run(self):
        instance = self.instance

        if instance.handler:
            if instance.handler.type_str == "device":
                instance.handler.suite = self.suite

            instance.handler.handle()

        sys.stdout.flush()

    def log_info_file(self):
        build_dir = self.instance.build_dir
        h_log = "{}/handler.log".format(build_dir)
        b_log = "{}/build.log".format(build_dir)
        d_log = "{}/device.log".format(build_dir)

        if os.path.exists(h_log) and os.path.getsize(h_log) > 0:
            logger.error("see: {}".format(h_log))
        elif os.path.exists(d_log) and os.path.getsize(d_log) > 0:
            logger.error("see: {}".format(d_log))
        else:
            logger.error("see: {}".format(b_log))

    def report_out(self, results):
        total_to_do = results.total - results.skipped_configs
        total_tests_width = len(str(total_to_do))
        results.done += 1
        instance = self.instance

        if instance.status in ["error", "failed", "timeout", "flash_error"]:
            if instance.status == "error":
                results.error += 1
            results.failed += 1
            logger.error(
                "{:<5} {:<3} {:<5} {:<50} FAILED: {}".format(
                    instance.platform.name,
                    instance.platform.version,
                    instance.platform.core,
                    instance.testcase.name,
                    instance.reason))
            self.log_info_file()
        if instance.status == "skipped":
            more_info = instance.reason
        else:
            if instance.handler and instance.run:
                more_info = instance.handler.type_str
                htime = instance.handler.duration
                if htime:
                    more_info += " {:.3f}s".format(htime)
            else:
                more_info = "build"

        logger.info("{:>{}}/{} {:<5} {:<3} {:<5} {:<50} {} ({})".format(
            results.done, total_tests_width, total_to_do, instance.platform.name,
            instance.platform.version, instance.platform.core,
            instance.testcase.name, instance.status, more_info))

        completed_perc = 0
        if total_to_do > 0:
            completed_perc = int((float(results.done) / total_to_do) * 100)

        skipped = results.skipped_configs + results.skipped_runtime
        logger.debug("Total complete: %4d/%4d  %2d%%  skipped: %4d, failed: %4d\n" % (
                results.done,
                total_to_do,
                completed_perc,
                skipped,
                results.failed
            )
        )


class TestSuite(DisablePyTestCollectionMixin):
    tc_white_black_list = yaml.safe_load(
        open(os.path.join(EMBARC_ROOT, "example", "examples.yaml"))
    )

    def __init__(self, testcase_roots=[], toolchain="mw", outdir=None):

        self.roots = testcase_roots
        self.build_only = False
        self.cleanup = False

        self.testcases = {}
        self.platforms = []
        self.toolchain = toolchain
        self.selected_platforms = []
        self.filtered_platforms = []
        self.default_platforms = []
        self.device_testing = False
        self.outdir = os.path.abspath(outdir)
        self.discards = {}
        self.instances = dict()

        self.total_platforms = 0
        self.start_time = 0
        self.duration = 0
        self.warnings = 0

        self.pipeline = None
        self.version = "NA"
        self.duts = []

    def check_embarc_osp_version(self):
        try:
            subproc = subprocess.run(["git", "rev-parse", "HEAD"],
                                     stdout=subprocess.PIPE,
                                     universal_newlines=True,
                                     cwd=EMBARC_ROOT)
            if subproc.returncode == 0:
                self.version = subproc.stdout.strip()
                logger.info(f"embarc_osp version: {self.version}")
        except OSError:
            logger.error("Cannot read embarc_osp version.")

    def add_testcases(self, testcase_filter=[]):
        supported_tc = list(self.tc_white_black_list["examples"].keys())
        for root in self.roots:
            root = os.path.abspath(root)
            logger.debug("Reading test case configuration files under %s..." % root)

            for dirpath, _, filenames in os.walk(root, topdown=True):
                for makefile in MAKEFILENAMES:
                    if makefile in filenames:
                        if is_embarc_osp_makefile(os.path.join(dirpath, makefile)):
                            logger.debug("Found test case in " + dirpath)
                            try:
                                workdir = os.path.relpath(dirpath, EMBARC_ROOT)
                                name = workdir.replace(os.path.sep, '/').replace("/", ".")
                                if name in supported_tc:
                                    tc = TestCase(root, workdir, name)
                                    tc_filter = self.tc_white_black_list["examples"][name]
                                    tc.tags = tc_filter.get("tags", set())
                                    tc.skip = tc_filter.get("skip", False)
                                    tc.extra_args = tc_filter.get("extra_args", [])
                                    tc_platform_exclude = tc_filter.get("platform_exclude", None)
                                    tc.build_only = tc_filter.get("build_only", False)
                                    if isinstance(tc_platform_exclude, str):
                                        tc.platform_exclude = [Platform(name) for name in tc_platform_exclude.split()]
                                    if isinstance(tc_platform_exclude, dict):
                                        tc.platform_exclude = list()
                                        for name, versions in tc_platform_exclude.items():
                                            versions_exclude = str(versions).split()
                                            for version in versions_exclude:
                                                tc.platform_exclude.append(Platform(name, version=version))
                                    tc_platform_allow = tc_filter.get("platform_allow", None)
                                    if isinstance(tc_platform_allow, str):
                                        tc.platform_allow = [Platform(name) for name in tc_platform_allow.split()]
                                    if isinstance(tc_platform_allow, dict):
                                        tc.platform_allow = list()
                                        for name, versions in tc_platform_allow.items():
                                            versions_allow = str(versions).split()
                                            for version in versions_allow:
                                                tc.platform_allow.append(Platform(name, version=version))
                                    tc.source_dir = dirpath
                                    tc.cases.append(tc.id)
                                    self.testcases[tc.name] = tc
                            except Exception as e:
                                logger.debug(e)
        return len(self.testcases)

    def update_counting(self, results=None, initial=False):
        results.skipped_configs = 0
        results.skipped_cases = 0
        for instance in self.instances.values():
            if initial:
                results.cases += len(instance.testcase.cases)
            if instance.status == 'skipped':
                results.skipped_configs += 1
                results.skipped_cases += len(instance.testcase.cases)
            elif instance.status == "passed":
                results.passed += 1
                for res in instance.results.values():
                    if res == 'SKIP':
                        results.skipped_cases += 1

    def add_instances(self, instance_list):
        for instance in instance_list:
            self.instances[instance.name] = instance

    def add_default_platforms(self, platforms=None):
        example_path = list(self.testcases.values())[0].source_dir
        for file in glob.glob(os.path.join(EMBARC_ROOT, "board", "*", "*.mk")):
            try:
                name = os.path.splitext(os.path.basename(file))[0]
                if platforms and name not in platforms:
                    continue
                platform = Platform(name)
                if platform.name == "nsim":
                    platform.simulation = "nsim"
                    platform.run = True
                versions = platform.get_versions(example_path)
                for version in versions:
                    cores = platform.get_cores(example_path, version)
                    for core in cores:
                        platform.version = version
                        platform.core = core
                        self.default_platforms.append(copy.deepcopy(platform))
            except RuntimeError as e:
                logger.error("E: %s: can't load: %s" % (file, e))

    def apply_filters(self, **kwargs):
        discards = {}
        platform_filter = kwargs.get('platform', [])
        platform_version_filter = kwargs.get('platform_version', [])
        platform_core_filter = kwargs.get('core', [])
        device_testing = kwargs.get('device_testing', False)
        device_serial = kwargs.get('device_serial', False)
        digilent_device = kwargs.get('digilent_device', '')

        if not platform_filter:
            for default_platform in self.default_platforms:
                self.platforms.append(default_platform)
        else:
            for default_platform in self.default_platforms:
                if default_platform.name in platform_filter:
                    if not platform_version_filter:
                        self.platforms.append(default_platform)
                    else:
                        if default_platform.version in platform_version_filter:
                            if not platform_core_filter:
                                self.platforms.append(default_platform)
                            else:
                                if default_platform.core in platform_core_filter:
                                    # only one connected device
                                    if device_testing and device_serial:
                                        default_platform.run = True
                                        default_platform.digilent_device = digilent_device
                                    self.platforms.append(default_platform)

        def sort_platforms(platforms):
            sorted_platforms = dict()
            if platforms is None:
                return sorted_platforms
            for plat in platforms:
                if sorted_platforms.get(plat.name, None) is None:
                    if plat.version is None:
                        sorted_platforms[plat.name] = list()
                    else:
                        sorted_platforms[plat.name] = [plat.version]
                else:
                    if plat.version is not None:
                        sorted_platforms[plat.name].append(plat.version)
            return sorted_platforms

        logger.info("Building initial testcase list...")
        for _, tc in self.testcases.items():
            instance_list = []

            platform_allow = sort_platforms(tc.platform_allow)
            platform_exclude = sort_platforms(tc.platform_exclude)

            for plat in self.platforms:
                instance = TestInstance(tc, plat, self.toolchain, self.outdir)
                instance.run = plat.run and (not tc.build_only)
                if tc.skip:
                    discards[instance] = discards.get(instance, "Skip filter")
                if platform_allow:
                    if plat.name not in platform_allow.keys():
                        discards[instance] = discards.get(instance, "Not in testcase platform allow list.")
                    if platform_allow.get(plat.name):
                        if plat.version not in platform_allow.get(plat.name):
                            discards[instance] = discards.get(instance, "Not in testcase platform version allow list.")
                if platform_exclude:
                    if isinstance(platform_exclude.get(plat.name), list):
                        if not platform_exclude.get(plat.name):
                            discards[instance] = discards.get(instance, "In testcase platform exlude list.")
                        if plat.version in platform_exclude.get(plat.name):
                            discards[instance] = discards.get(instance, "In testcase platform version exlude list.")

                for t in tc.cases:
                    instance.results[t] = None
                instance_list.append(instance)

            if not instance_list:
                continue
            self.add_instances(instance_list)

        self.selected_platforms = set(p.platform for p in self.instances.values())
        self.discards = discards
        for instance in self.discards:
            instance.reason = self.discards[instance]
            instance.status = "skipped"
            instance.fill_results_by_status()
        return discards

    def add_tasks_to_queue(self, pipeline, build_only=False):
        for instance in self.instances.values():
            os.makedirs(instance.build_dir, exist_ok=True)
            if build_only:
                instance.run = False

            if instance.status not in ['passed', 'skipped', 'error']:
                instance.status = None
                pipeline.put({"op": "build", "test": instance})

    def pipeline_mgr(self, pipeline, done_queue, lock, results):
        while True:
            try:
                task = pipeline.get_nowait()
            except queue.Empty:
                break
            else:
                test = task['test']
                pb = ProjectBuilder(self,
                                    test,
                                    device_testing=self.device_testing
                                    )
                pb.process(pipeline, done_queue, task, lock, results)

        return True

    def execute(self, pipeline, done, results):
        lock = Lock()
        logger.debug("Adding tasks to the queue...")
        self.add_tasks_to_queue(pipeline, self.build_only)
        logger.debug("Added initial list of jobs to queue")

        processes = []
        for job in range(self.jobs):
            logger.debug(f"Launch process {job}")
            p = Process(target=self.pipeline_mgr, args=(pipeline, done, lock, results, ))
            processes.append(p)
            p.start()

        try:
            for p in processes:
                p.join()
        except KeyboardInterrupt:
            logger.error("Execution interrupted")
            for p in processes:
                p.terminate()

        for instance in self.instances.values():
            instance.metrics["ram_size"] = 0
            instance.metrics["rom_size"] = 0
            instance.metrics["handler_time"] = instance.handler.duration if instance.handler else 0
            instance.metrics["unrecognized"] = []

        return results

    def summary(self, results):
        failed = 0
        run = 0
        for instance in self.instances.values():
            if instance.status == "failed":
                failed += 1
            elif instance.metrics.get("unrecognized"):
                failed += 1

            if instance.metrics.get('handler_time', None):
                run += 1

        if results.total and results.total != results.skipped_configs:
            pass_rate = (float(results.passed) / float(results.total - results.skipped_configs))
        else:
            pass_rate = 0

        logger.info(
            "{} of {} test configurations passed ({:.2%}), {} failed, {} skipped with {} warnings in {:.2f} seconds\n".format(
                results.passed,
                results.total - results.skipped_configs,
                pass_rate,
                results.failed,
                results.skipped_configs,
                self.warnings,
                self.duration))

        self.total_platforms = len(self.platforms)
        if self.platforms and not self.build_only:
            logger.info("In total {} test cases were executed, {} skipped on {} out of total {} platforms ({:02.2f}%)\n".format(
                results.cases - results.skipped_cases,
                results.skipped_cases,
                len(self.filtered_platforms),
                self.total_platforms,
                (100 * len(self.filtered_platforms) / len(self.platforms))
            ))

        logger.info(f"{run} test configurations executed on platforms, {results.total - run - results.skipped_configs} test configurations were only built.\n")
        sys.stdout.flush()

    @staticmethod
    def process_log(log_file):
        filtered_string = ""
        if os.path.exists(log_file):
            with open(log_file, "rb") as f:
                log = f.read().decode("utf-8")
                filtered_string = ''.join(filter(lambda x: x in string.printable, log))

        return filtered_string

    def save_reports(self, report_dir):
        report = {}
        testcases = []
        os.makedirs(report_dir, exist_ok=True)
        filename = os.path.join(report_dir, "osp_test.json")
        logger.info(f"Writing JSON report {filename}")

        selected = self.selected_platforms
        report["environment"] = {"os": os.name,
                                 "osp_version": self.version,
                                 "toolchain": self.toolchain
                                 }
        for p in selected:
            inst = {k: v for k, v in self.instances.items() if k.startswith(os.path.join(p.name, p.version, p.core))}
            for _, instance in inst.items():
                testcase = {}
                handler_log = os.path.join(instance.build_dir, "handler.log")
                build_log = os.path.join(instance.build_dir, "build.log")
                device_log = os.path.join(instance.build_dir, "device.log")

                handler_time = instance.metrics.get('handler_time', 0)
                for k in instance.results.keys():
                    testcases = list(filter(lambda d: not (d.get('testcase') == k and d.get('platform') == p), testcases))
                    testcase = {"testcase": k,
                                "platform": p.name,
                                "platform-version": p.version,
                                "core": p.core
                                }
                    if instance.results[k] in ["PASS"]:
                        testcase["status"] = "passed"
                        if instance.handler:
                            testcase["execution_time"] = handler_time

                    elif instance.results[k] in ['FAIL', 'BLOCK'] or instance.status in ["error", "failed", "timeout"]:
                        testcase["status"] = "failed"
                        testcase["reason"] = instance.reason
                        testcase["execution_time"] = handler_time
                        if os.path.exists(handler_log):
                            testcase["test_output"] = self.process_log(handler_log)
                        elif os.path.exists(device_log):
                            testcase["device_log"] = self.process_log(device_log)
                        else:
                            testcase["build_log"] = self.process_log(build_log)
                    else:
                        if instance.run:
                            testcase["status"] = "skipped"
                        else:
                            testcase["status"] = instance.status
                        testcase["reason"] = instance.reason
                    testcases.append(testcase)

        suites = [{"build_only": self.build_only, "testcases": testcases}]
        report["testsuites"] = suites

        with open(filename, "wt") as json_file:
            json.dump(report, json_file, indent=4, separators=(',', ':'))


def main():
    start_time = time.time()
    options = parse_arguments()

    ch = logging.StreamHandler()
    if options.verbose > 1:
        ch.setLevel(logging.DEBUG)
    else:
        ch.setLevel(logging.INFO)
    ch.setFormatter(logging.Formatter('%(levelname)-7s - %(message)s'))
    logger.addHandler(ch)

    if os.path.exists(options.outdir):
        shutil.rmtree(options.outdir)
    os.makedirs(options.outdir, exist_ok=True)
    hwm = HardwareMap()
    if options.device_testing and not options.build_only:
        if not (len(options.platform) == 1 and len(options.platform_version) == 1 and len(options.core)):
            logging.error("""When --device-testing is used with --device-serial
                  only one platform is allowed""")
            sys.exit(1)
        else:
            hwm.add_device(options.device_serial,
                           options.platform[0],
                           None)
    if not options.testcase_root:
        options.testcase_root = [os.path.join(EMBARC_ROOT, "example")]

    suite = TestSuite(options.testcase_root, options.toolchain, options.outdir)
    suite.build_only = options.build_only
    suite.duts = hwm.duts
    suite.check_embarc_osp_version()
    suite.device_testing = options.device_testing
    if options.jobs:
        suite.jobs = options.jobs
    elif options.build_only:
        suite.jobs = multiprocessing.cpu_count() * 2
    else:
        suite.jobs = multiprocessing.cpu_count()
    suite.add_testcases()
    suite.add_default_platforms(options.platform)
    suite.apply_filters(
        platform=options.platform,
        platform_version=options.platform_version,
        core=options.core,
        device_testing=options.device_testing,
        device_serial=options.device_serial,
        digilent_device=options.digilent_device
    )

    BaseManager.register('LifoQueue', queue.LifoQueue)
    manager = BaseManager()
    manager.start()

    results = ExecutionCounter(total=len(suite.instances))
    pipeline = manager.LifoQueue()
    done_queue = manager.LifoQueue()

    suite.update_counting(results, initial=True)
    suite.start_time = start_time

    results = suite.execute(pipeline, done_queue, results)

    while True:
        try:
            inst = done_queue.get_nowait()
        except queue.Empty:
            break
        else:
            inst.metrics["handler_time"] = inst.handler.duration if inst.handler else 0
            inst.metrics["unrecognized"] = []
            suite.instances[inst.name] = inst
    suite.duration = time.time() - start_time
    suite.update_counting(results)
    suite.summary(results)
    report_dir = options.outdir
    if options.report_dir:
        report_dir = options.report_dir
    suite.save_reports(report_dir)


if __name__ == "__main__":
    main()
