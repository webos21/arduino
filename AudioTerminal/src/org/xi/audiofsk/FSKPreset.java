package org.xi.audiofsk;

/**
 * Preset values for FSKModem. (from the C Macros : #define)
 */
interface FSKPreset {
	interface Baud126r1 {
		public final static int FREQ_LOW = 882;
		public final static int FREQ_HIGH = 1764;
		public final static int BAUD = 126;
	}

	interface Baud126r2 {
		public final static int FREQ_LOW = 630;
		public final static int FREQ_HIGH = 1260;
		public final static int BAUD = 126;
	}

	interface Baud100 {
		public final static int FREQ_LOW = 800;
		public final static int FREQ_HIGH = 1600;
		public final static int BAUD = 100;
	}

	interface Baud315 {
		public final static int FREQ_LOW = 1575;
		public final static int FREQ_HIGH = 3150;
		public final static int BAUD = 315;
	}

	interface Baud600 {
		public final static int FREQ_LOW = 2666;
		public final static int FREQ_HIGH = 4000;
		public final static int BAUD = 600;
	}

	interface Baud630 {
		public final static int FREQ_LOW = 3150;
		public final static int FREQ_HIGH = 6300;
		public final static int BAUD = 630;
	}

	interface Baud1225 {
		public final static int FREQ_LOW = 4900;
		public final static int FREQ_HIGH = 7350;
		public final static int BAUD = 1225;
	}
}
