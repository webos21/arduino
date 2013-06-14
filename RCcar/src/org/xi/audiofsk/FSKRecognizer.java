package org.xi.audiofsk;

import java.io.ByteArrayOutputStream;
import java.util.ArrayList;

class FSKRecognizer {
	private final static int FSK_SMOOTH = 3;

	private final static int SMOOTHER_COUNT = (FSK_SMOOTH * (FSK_SMOOTH + 1) / 2);

	private final static double DISCRIMINATOR = (SMOOTHER_COUNT
			* (FSKConst.WL_HIGH + FSKConst.WL_LOW) / 4);

	private enum FSKRecState {
		FSKStart, FSKBits, FSKSuccess, FSKFail
	};

	private int recentLows;
	private int recentHighs;
	private int[] halfWaveHistory;
	private int bitPosition;
	private int recentWidth;
	private int recentAvrWidth;
	private byte bits;
	private FSKRecState state;

	private ByteArrayOutputStream _recvBuf;

	private ArrayList<FSKModemListener> _dataReceivers;

	public FSKRecognizer() {
		halfWaveHistory = new int[FSK_SMOOTH];
		_recvBuf = new ByteArrayOutputStream();
		_dataReceivers = new ArrayList<FSKModemListener>();
	}

	private void commitBytes() {
		synchronized (_recvBuf) {
			StringBuilder sb = new StringBuilder();
			byte[] inputBytes = _recvBuf.toByteArray();
			if (FSKConfig.logFlag) {
				sb.append("[Bytes] ------------------------------------\n");
				for (int i = 0; inputBytes != null & i < inputBytes.length; i++) {
					int v = inputBytes[i] & 0xff;
					if (v > 31 && v < 127) {
						sb.append((char) v);
					} else {
						if (v < 16) {
							sb.append(" 0");
							sb.append(Integer.toHexString(v));
							sb.append(' ');
						} else {
							sb.append(' ');
							sb.append(Integer.toHexString(v));
							sb.append(' ');
						}
					}
				}
				sb.append('\n');
				sb.append("--------------------------------------------\n");
				FSKLog.log(FSKLog.DEBUG, sb.toString());
			}
			synchronized (_dataReceivers) {
				for (FSKModemListener lsn : _dataReceivers) {
					lsn.dataReceivedFromFSKModem(inputBytes);
				}
			}
			_recvBuf.reset();
		}
	}

	private void dataBit(boolean one) {
		if (one) {
			bits |= (1 << bitPosition);
		}
		bitPosition++;
	}

	private void freqRegion(int length, boolean high) {
		FSKRecState newState = FSKRecState.FSKFail;
		switch (state) {
		case FSKStart:
			if (!high) { // Start Bit
				if (FSKConfig.logFlag) {
					StringBuilder sb = new StringBuilder();
					sb.append("Start bit: ").append(high ? 'H' : 'L')
							.append(" ").append(length);
					FSKLog.log(FSKLog.VERBOSE, sb.toString());
				}
				newState = FSKRecState.FSKBits;
				bits = 0;
				bitPosition = 0;
			} else {
				newState = FSKRecState.FSKStart;
			}
			break;
		case FSKBits:
			if (FSKConfig.logFlag) {
				StringBuilder sb = new StringBuilder();
				sb.append("Bit: ").append(high ? 'H' : 'L').append(" ")
						.append(length);
				FSKLog.log(FSKLog.VERBOSE, sb.toString());
			}
			if ((bitPosition >= 0) && (bitPosition <= 7)) { // Data Bits
				newState = FSKRecState.FSKBits;
				dataBit(high);
			} else if (bitPosition == 8) { // Stop Bit
				newState = FSKRecState.FSKStart;
				_recvBuf.write(bits);
				commitBytes();
				bits = 0;
				bitPosition = 0;
			}
			break;
		default:
			break;
		}
		state = newState;
	}

	private void halfWave(long width) {
		for (int i = FSK_SMOOTH - 2; i >= 0; i--) {
			halfWaveHistory[i + 1] = halfWaveHistory[i];
		}
		halfWaveHistory[0] = (int) width;

		int waveSum = 0;
		for (int i = 0; i < FSK_SMOOTH; ++i) {
			waveSum += halfWaveHistory[i] * (FSK_SMOOTH - i);
		}

		boolean high = (waveSum < DISCRIMINATOR);
		int avgWidth = waveSum / SMOOTHER_COUNT;

		recentWidth += width;
		recentAvrWidth += avgWidth;

		if (FSKConfig.logFlag) {
			StringBuilder sb = new StringBuilder();
			sb.append("high = ").append(high);
			sb.append(" / width = ").append(width / 1000);
			sb.append(" / avg = ").append(avgWidth / 1000);
			sb.append(" / low = ").append(recentLows / 1000);
			sb.append(" / high = ").append(recentHighs / 1000);
			sb.append(" / w = ").append(recentWidth / 1000);
			sb.append(" / a = ").append(recentAvrWidth / 1000);
			sb.append("\n");
			FSKLog.log(FSKLog.VERBOSE, sb.toString());
		}

		if (state == FSKRecState.FSKStart) {
			if (!high) {
				recentLows += avgWidth;
			} else if (recentLows != 0) {
				recentHighs += avgWidth;
				if (recentHighs > recentLows) {
					if (FSKConfig.logFlag) {
						FSKLog.log(FSKLog.VERBOSE, "False start = "
								+ recentLows);
					}
					recentLows = recentHighs = 0;
				}
			}

			if (recentLows + recentHighs >= FSKConst.BIT_PERIOD) {
				freqRegion(recentLows, high);
				recentWidth = recentAvrWidth = 0;
				if (recentLows < FSKConst.BIT_PERIOD) {
					recentLows = 0;
				} else {
					recentLows -= FSKConst.BIT_PERIOD;
				}
				if (!high) {
					recentHighs = 0;
				}
			}
		} else {
			if (high) {
				recentHighs += avgWidth;
			} else {
				recentLows += avgWidth;
			}

			if (recentLows + recentHighs >= FSKConst.BIT_PERIOD) {
				boolean regionHigh = (recentHighs > recentLows);
				freqRegion(regionHigh ? recentHighs : recentLows, regionHigh);

				recentWidth -= FSKConst.BIT_PERIOD;
				recentAvrWidth -= FSKConst.BIT_PERIOD;

				if (state == FSKRecState.FSKStart) {
					// The byte ended, reset the accumulators
					recentLows = recentHighs = 0;
					return;
				}

				if (regionHigh) {
					if (recentHighs < FSKConst.BIT_PERIOD) {
						recentHighs = 0;
					} else {
						recentHighs -= FSKConst.BIT_PERIOD;
					}
					if (high == regionHigh) {
						recentLows = 0;
					}
				} else {
					if (recentLows < FSKConst.BIT_PERIOD) {
						recentLows = 0;
					} else {
						recentLows -= FSKConst.BIT_PERIOD;
					}
					if (high == regionHigh) {
						recentHighs = 0;
					}
				}
			}
		}
	}

	private void reset() {
		bits = 0;
		bitPosition = 0;
		state = FSKRecState.FSKStart;
		for (int i = 0; i < FSK_SMOOTH; i++) {
			halfWaveHistory[i] = (int) ((FSKConst.WL_HIGH + FSKConst.WL_LOW) / 4);
		}
		recentLows = recentHighs = 0;
	}

	void edge(long nsWidth, long nsInterval) {
		if (nsInterval <= FSKConst.HWL_LOW + FSKConst.HWL_HIGH) {
			halfWave(nsInterval);
		} else {
			if (FSKConfig.logFlag) {
				FSKLog.log(FSKLog.VERBOSE, "Recognizer.edge() : skip!!!!!!");
			}
		}
	}

	void idle(long nsInterval) {
		reset();
	}

	void release() {
		_recvBuf.reset();
		_recvBuf = null;

		_dataReceivers.clear();
		_dataReceivers = null;
	}

	boolean addDataReceiver(FSKModemListener lsn) {
		synchronized (_dataReceivers) {
			return _dataReceivers.add(lsn);
		}
	}

	boolean removeDataReceiver(FSKModemListener lsn) {
		synchronized (_dataReceivers) {
			return _dataReceivers.remove(lsn);
		}
	}
}
