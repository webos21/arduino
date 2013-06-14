package org.xi.audiofsk;

/**
 * Constant values for FSKModem (from the C Macros : #define)
 */
interface FSKConst {
	int SAMPLE_RATE = FSKConfig.SAMPLE_RATE;
	int SAMPLE_SIZE = Short.SIZE / 8; // 2 bytes (the size of the short integer)
	int SAMPLE_MAX = Short.MAX_VALUE;
	int NUM_CHANNELS = 1; // MONO

	int BITS_PER_CHANNEL = (SAMPLE_SIZE * 8 / NUM_CHANNELS);
	int BYTES_PER_FRAME = (NUM_CHANNELS * SAMPLE_SIZE);

	double WL_HIGH = (1000000000.0 / FSKConfig.FREQ_HIGH);
	double WL_LOW = (1000000000.0 / FSKConfig.FREQ_LOW);
	double HWL_HIGH = (500000000.0 / FSKConfig.FREQ_HIGH);
	double HWL_LOW = (500000000.0 / FSKConfig.FREQ_LOW);

	double BIT_PERIOD = (1000000000.0 / FSKConfig.BAUD);

	int SINE_TB_LEN = 441;

	double PRE_CARRIER_BITS = ((40000000.0 + BIT_PERIOD) / BIT_PERIOD);
	double POST_CARRIER_BITS = ((5000000.0 + BIT_PERIOD) / BIT_PERIOD);

	// TABLE_JUMP = phase_per_sample / phase_per_entry
	// phase_per_sample = 2pi * time_per_sample / time_per_wave
	// phase_per_entry = 2pi / SINE_TABLE_LENGTH
	// TABLE_JUMP = time_per_sample / time_per_wave * SINE_TABLE_LENGTH
	// time_per_sample = 1000000000 / SAMPLE_RATE
	// time_per_wave = 1000000000 / FREQ
	// TABLE_JUMP = FREQ / SAMPLE_RATE * SINE_TABLE_LENGTH
	double TABLE_JUMP_HIGH = (FSKConfig.FREQ_HIGH * SINE_TB_LEN / SAMPLE_RATE);
	double TABLE_JUMP_LOW = (FSKConfig.FREQ_LOW * SINE_TB_LEN / SAMPLE_RATE);

	double SAMPLE_DURATION = (1000000000.0 / SAMPLE_RATE);

	// Constants for Demodulation {
	int EDGE_DIFF_THRESHOLD = 16384;
	int EDGE_SLOPE_THRESHOLD = 256;
	int EDGE_MAX_WIDTH = 8;
	int IDLE_CHECK_PERIOD = (int) ((10 * SAMPLE_RATE) / 1000.0); // MS_TO_SAMPLES(10)
	// }
}
