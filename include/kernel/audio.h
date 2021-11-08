/**
 * @addtogroup drv_audio
 * @{
 */
#pragma once
#include <kernel/packet.h>

struct audio;

enum audio_output_mode {
    /** Audio output in PWM mode on pins PWM0-3 */
    AUDIO_OUT_PWM = 0,
    /** Audio output using I2S interface pins */
    AUDIO_OUT_I2S = 1
};

struct audio *audio_open_playback(enum audio_output_mode mode, uint32_t rate, uint32_t flags);
void audio_set_callback(struct audio *stream, void (*complete_cb)(struct audio *));
void audio_set_volume(struct audio *stream, int percent);
int audio_write_packet(struct audio *stream, struct packet *pkt);
void audio_close(struct audio *stream);

/** @} */
