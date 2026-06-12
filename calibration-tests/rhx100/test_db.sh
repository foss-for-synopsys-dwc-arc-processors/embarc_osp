METRICS_ARRAY=("-1" "-1" "-1" "-1" "-1" "unknown")
{
    printf "passed:%s::" "${METRICS_ARRAY[0]}"
    printf "skipped:%s::" "${METRICS_ARRAY[3]}"
    printf "failed:%s::" "${METRICS_ARRAY[1]}"
    printf "error:%s::" "${METRICS_ARRAY[2]}"
    printf "only_build:%s::" "${METRICS_ARRAY[4]}"
    printf "handler_time:%s::" "0"
} > "resultsdb_metrics_file.log"