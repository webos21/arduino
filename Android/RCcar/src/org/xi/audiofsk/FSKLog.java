package org.xi.audiofsk;

import android.util.Log;

class FSKLog {
	public static final int VERBOSE = Log.VERBOSE;
	public static final int DEBUG = Log.DEBUG;
	public static final int INFO = Log.INFO;
	public static final int WARN = Log.WARN;
	public static final int ERROR = Log.ERROR;
	public static final int ASSERT = Log.ASSERT;

	public static void log(int level, String msg) {
		if (FSKConfig.logFlag) {
			if (level >= FSKConfig.logLevel) {
				Log.println(level, FSKConfig.logTag, msg);
			}
		}
	}

	public static void log(int level, String msg, Throwable tr) {
		if (FSKConfig.logFlag) {
			if (level >= FSKConfig.logLevel) {
				Log.println(level, FSKConfig.logTag, msg);
				Log.println(level, FSKConfig.logTag,
						Log.getStackTraceString(tr));
			}
		}
	}

}
