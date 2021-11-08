#pragma once

/** Start the iperf3 server
 *  Set stream_cb to get a callback when the stream is started or ended.
 */
void iperf3_server(void(*stream_cb)(bool started));
