# Xively C library (libxively)

## Beta Release Notes

  The goals of the design had been focused on supporting embedded platforms
  and providing simple wrapper methods that will enable users to implement
  a very functional Xively agent application on an embedded SoC or MCU device.

  Our design has a very well defined layering model.
  It is currently comprised of the following layers:

  * _communication layer_ - wraps platform-specific networking functions
    (to open socket connections, read and send data, close sockets)
  * _transport layer_ - encode/decode protocol-level requests/responses (e.g. HTTP or WebSocket)
  * _data layer_ - encodes/decodes Xively data formats (e.g. CSV or JSON)

  Given the resource constraints of embedded clients and the typical usage of
  the Xively API in such end-devices, we have not implemented methods to cover
  functionalities such as the historic queries or feed search at this point.
  Also methods for creating and deleting feeds are not provided, as the
  end-device should use provisioning API, which will be implemented in the
  upcoming version of the library.

  At the present time there is no JSON support in this library. If users need
  JSON support, the CSV layer can be replaced easily. With so many JSON
  libraries to chose from with varying performance, size and features, we have
  intentionally chosen to not to include a JSON library. As we advance the
  library, JSON support may be introduced.

  Please watch this repository on GitHub to be first to find out of any
  upcoming features. Make sure to submit your feedback via [an issue
  ticket][newissue], [tweet][atxively] or email support@xively.com.


## Get started

Run this from the shell:

      git clone --recursive https://github.com/xively/libxively
      cd libxively
      make all

You will find compiled examples under `src/bin`, which you can run if you
like, however we recommend to read the source code first. Have fun!

## Stability
<table>
<tr>
<td><b><i>Build status</i></b></td><td><a href="https://travis-ci.org/xively/libxively"><img src="https://travis-ci.org/xively/libxively.png?branch=master" /></a></td>
</tr>
<tr>
<td><b><i>Development</i></b></td><td><i>ongoing</i></td>
</tr>
</table>

  This library is continuously tested on [Travis CI][travis] as well as Xively's
  internal infrastructure, therefore it is rather safe to use the latest revision.
  Users may need to make changes to their code, as minor improvements are likely
  to be introduced in the library functions calls until _1.0.0_ and the following
  major releases.

## Support

  Any issues that appear to be a bug in the library code, examples or documentation,
  should be submitted via GitHub and should get fixed once confirmed and understood.
  Feature request should be submitted via GitHub also.
  Any coding help is best to find on [Stack Overflow][so], where you will get answers
  from ourselves as the entire community of professionals. For commercial needs, please
  refer to [Xively website][pricing].

## Contributing

  We use GitHub fork/pull-request model and encourage users to contribute
  changes back. The general guideline is to submit no more then one feature
  in a single pull-request and each individual commit should contain only
  related changes.

  _TBD: Code style._

  Documentation is generated using latest version of Doxygen with markdown
  support, which is often a little bit different with GitHub's markdown.
  You should check for output on both when making changes to the docs.

  Commit message should begin with one of the following tags enclosed in
  square brackets: _fix, feature, docs, build, style_. You shouldn't need to use
  more then three tags. If the message contains additional description, put
  a dash at the end of the summary line. There are also layer and platform

  Here is a single-line commit message with platform and layer tags:

      [fix][comm][mbed] Check for errors after creating a socket (close #123)


  As you can see, the command `close #123` should be placed in braces if you
  intend to close GitHub issue `#123`.

  Here is a feature story with it's acceptance criteria and some notes:

      [feature][comm][uip] Basic implementation -
  
        * tested with 1.0
        * has simple error handling
    
        This needs more testing.

A few more:

      [style] Strip whitespaces and double blank lines
      [build] Fix issues with armcc

[travis]: http://travis-ci.org/xively/libxively
[so]: http://stackoverflow.com/
[pricing]: https://xively.com/pricing/#support
[newissue]: https://github.com/xively/libxively/issues/new
[atxively]: https://tweetter.com/errordeveloper

[![githalytics.com alpha](https://cruel-carlota.pagodabox.com/ac9aa22a4a17f673e8988c5037946a4a "githalytics.com")](http://githalytics.com/xively/libxively)
