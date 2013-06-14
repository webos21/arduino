package org.xi.audiofsk;

/**
 * Configuration values for FSKModem (from the C Macros : #define)
 */
interface FSKConfig {

	// Baud-Rate & Frequencies {
	public final static int FREQ_LOW = FSKPreset.Baud1225.FREQ_LOW;
	public final static int FREQ_HIGH = FSKPreset.Baud1225.FREQ_HIGH;
	public final static int BAUD = FSKPreset.Baud1225.BAUD;
	// }

	// Sampling-Rate for Audio In/Out {
	public final static int SAMPLE_RATE = 44100;
	// }

	// Log Constants {
	public static final String logTag = "FSKModem";
	public static final boolean logFlag = true;
	public static final int logLevel = FSKLog.INFO;
	// }
}
