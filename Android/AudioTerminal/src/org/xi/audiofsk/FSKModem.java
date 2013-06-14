package org.xi.audiofsk;

import java.util.ArrayList;

public class FSKModem implements FSKDataInterface {

	// ////////////////////////////////////////////////////
	// Inner Classes for MODEM data
	// ////////////////////////////////////////////////////

	class ModulationData {
		public boolean byteUnderflow;
		public boolean sendCarrier;

		public int sineTableIndex;
		public int bitCount;
		public double nsBitProgress;
		public short bits;

		public ModulationData() {
			byteUnderflow = true;
		}
	}

	class DemodulationData {
		public int lastFrame;
		public int lastEdgeSign;
		public int lastEdgeWidth;
		public int edgeSign;
		public int edgeDiff;
		public int edgeWidth;
		public int plateauWidth;
		// DETAILED_ANALYSIS {
		public long edgeSum;
		public int edgeMin;
		public int edgeMax;
		public long plateauSum;
		public int plateauMin;
		public int plateauMax;
		// }
		
		public DemodulationData() {
		}
	}

	// ////////////////////////////////////////////////////
	// Member variables
	// ////////////////////////////////////////////////////

	private short[] _sineTable;

	private ModulationData _modData;

	private DemodulationData _demodData;

	private ArrayList<Byte> _sendBuf;

	private FSKRecognizer _recognizer;

	private AndroidAudio _audioIO;

	// ////////////////////////////////////////////////////
	// Constructor
	// ////////////////////////////////////////////////////

	public FSKModem() {
		_sineTable = new short[FSKConst.SINE_TB_LEN];
		for (int i = 0; i < FSKConst.SINE_TB_LEN; ++i) {
			_sineTable[i] = (short) (Math.sin(i * 2 * Math.PI
					/ FSKConst.SINE_TB_LEN) * FSKConst.SAMPLE_MAX);
		}

		_modData = new ModulationData();
		_demodData = new DemodulationData();

		_sendBuf = new ArrayList<Byte>();

		_recognizer = new FSKRecognizer();

		_audioIO = new AndroidAudio();
	}

	private byte getNextByte() {
		byte data;
		if (_modData.byteUnderflow) {
			if (!_sendBuf.isEmpty()) {
				_modData.bits = 1;
				_modData.bitCount = (int) FSKConst.PRE_CARRIER_BITS;
				_modData.sendCarrier = true;
				_modData.byteUnderflow = false;
				return 1;
			}
		} else {
			if (!_sendBuf.isEmpty()) {
				data = _sendBuf.remove(0);
				_modData.bits = (short) ((data << 1) | (0x0200));
				_modData.bitCount = 10;
				_modData.sendCarrier = false;
				return 1;
			} else {
				_modData.bits = 1;
				_modData.bitCount = (int) FSKConst.POST_CARRIER_BITS;
				_modData.sendCarrier = true;
				_modData.byteUnderflow = true;
				return 1;
			}
		}
		_modData.bits = 1; // Make sure the output is HIGH when there is no data
		return 0;
	}

	private void updatePlayData(short[] audioData, int dataLen) {
		synchronized (_sendBuf) {

			boolean underflow = false;

			if (_modData.bitCount == 0) {
				underflow = (getNextByte() == 0) ? true : false;
			}

			for (int i = 0; i < dataLen; i++) {
				if (_modData.nsBitProgress >= FSKConst.BIT_PERIOD) {
					if (_modData.bitCount > 0) {
						--_modData.bitCount;
						if (!_modData.sendCarrier) {
							_modData.bits >>= 1;
						}
					}
					_modData.nsBitProgress -= FSKConst.BIT_PERIOD;
					if (_modData.bitCount == 0) {
						underflow = (getNextByte() == 0) ? true : false;
					}
				}

				if (underflow) {
					audioData[i] = 0;
				} else {
					_modData.sineTableIndex += ((_modData.bits & 1) == 1) ? FSKConst.TABLE_JUMP_HIGH
							: FSKConst.TABLE_JUMP_LOW;
					if (_modData.sineTableIndex >= FSKConst.SINE_TB_LEN) {
						_modData.sineTableIndex -= FSKConst.SINE_TB_LEN;
					}
					audioData[i] = _sineTable[_modData.sineTableIndex];
				}

				if (_modData.bitCount != 0) {
					_modData.nsBitProgress += FSKConst.SAMPLE_DURATION;
				}
			}
		}
	}

	private void edgeHeight(int width, int interval) {
		// Convert to microseconds
		long nsInterval = SAMPLES_TO_NS(interval);
		long nsWidth = SAMPLES_TO_NS(width);
		_recognizer.edge(nsWidth, nsInterval);
	}

	private void idle(int samples) {
		long nsInterval = SAMPLES_TO_NS(samples);
		_recognizer.idle(nsInterval);
	}

	private void processRecData(short[] audioData, int len) {
		if (len <= 0) {
			return;
		}

		DemodulationData data = _demodData;
		short[] pSample = audioData;
		int lastFrame = _demodData.lastFrame;

		int idleInterval = data.plateauWidth + data.lastEdgeWidth
				+ data.edgeWidth;

		for (int i = 0; i < len; i++) {
			int thisFrame = pSample[i];
			int diff = thisFrame - lastFrame;

			int sign = 0;
			if (diff > FSKConst.EDGE_SLOPE_THRESHOLD) {
				// Signal is rising
				sign = 1;
			} else if (-diff > FSKConst.EDGE_SLOPE_THRESHOLD) {
				// Signal is falling
				sign = -1;
			}

			// If the signal has changed direction or the edge detector has gone
			// on for too long,
			// then close out the current edge detection phase
			if (data.edgeSign != sign
					|| ((data.edgeSign != 0) && ((data.edgeWidth + 1) > FSKConst.EDGE_MAX_WIDTH))) {
				if (Math.abs(data.edgeDiff) > FSKConst.EDGE_DIFF_THRESHOLD
						&& data.lastEdgeSign != data.edgeSign) {
					// The edge is significant
					edgeHeight(data.edgeWidth, data.plateauWidth
							+ data.edgeWidth);

					// Save the edge
					data.lastEdgeSign = data.edgeSign;
					data.lastEdgeWidth = data.edgeWidth;

					// Reset the plateau
					data.plateauWidth = 0;
					idleInterval = data.edgeWidth;
					// DETAILED_ANALYSIS {
					data.plateauSum = 0;
					data.plateauMin = data.plateauMax = thisFrame;
					// }
				} else {
					// The edge is rejected; add the edge data to the plateau
					data.plateauWidth += data.edgeWidth;
					// DETAILED_ANALYSIS {
					data.plateauSum += data.edgeSum;
					if (data.plateauMax < data.edgeMax) {
						data.plateauMax = data.edgeMax;
					}
					if (data.plateauMin > data.edgeMin) {
						data.plateauMin = data.edgeMin;
					}
					// }
				}

				data.edgeSign = sign;
				data.edgeWidth = 0;
				data.edgeDiff = 0;
				// DETAILED_ANALYSIS {
				data.edgeSum = 0;
				data.edgeMin = data.edgeMax = lastFrame;
				// }
			}

			if (data.edgeSign != 0) {
				// Sample may be part of an edge
				data.edgeWidth++;
				data.edgeDiff += diff;
				// DETAILED_ANALYSIS {
				data.edgeSum += thisFrame;
				if (data.edgeMax < thisFrame) {
					data.edgeMax = thisFrame;
				}
				if (data.edgeMin > thisFrame) {
					data.edgeMin = thisFrame;
				}
				// }
			} else {
				// Sample is part of a plateau
				data.plateauWidth++;
				// DETAILED_ANALYSIS {
				data.plateauSum += thisFrame;
				if (data.plateauMax < thisFrame) {
					data.plateauMax = thisFrame;
				}
				if (data.plateauMin > thisFrame) {
					data.plateauMin = thisFrame;
				}
				// }
			}
			idleInterval++;

			data.lastFrame = lastFrame = thisFrame;

			if ((idleInterval % FSKConst.IDLE_CHECK_PERIOD) == 0) {
				idle(idleInterval);
			}
		}
	}

	public void start() {
		_audioIO.setFSKDataInterface(this);
		_audioIO.startIO();
	}

	public void stop() {
		_audioIO.stopIO();
	}

	public void writeBytes(byte[] data) {
		synchronized (_sendBuf) {
			_sendBuf.add((byte) 0xff);
			for (int i = 0; i < data.length; i++) {
				_sendBuf.add(data[i]);
			}
		}
	}

	// ////////////////////////////////////////////////////
	// Add & Remove FSKModemListener
	// ////////////////////////////////////////////////////

	public boolean addDataReceiver(FSKModemListener lsn) {
		return _recognizer.addDataReceiver(lsn);
	}

	public boolean removeDataReceiver(FSKModemListener lsn) {
		return _recognizer.removeDataReceiver(lsn);
	}

	// ////////////////////////////////////////////////////
	// From org.xi.audiofsk.FSKDataInterface
	// ////////////////////////////////////////////////////

	@Override
	public void feedPlayData(short[] audioData, int dataLen) {
		updatePlayData(audioData, dataLen);
	}

	@Override
	public void pullRecData(short[] audioData, int dataLen) {
		processRecData(audioData, dataLen);
	}

	// ////////////////////////////////////////////////////
	// Functions for Debugging
	// ////////////////////////////////////////////////////

	public void debugPrintSineTable(StringBuilder sb) {
		if (FSKConfig.logFlag) {
			for (int i = 0; i < FSKConst.SINE_TB_LEN; ++i) {
				sb.append(_sineTable[i]);
				sb.append(", ");
			}
		}
	}

	public static void debugPrint(FSKModem fskm) {
		if (FSKConfig.logFlag) {
			final char NLC = '\n';
			final String SEP = "-----------------------------------------\n";

			StringBuilder sb = new StringBuilder();

			sb.append(SEP);
			sb.append("FREQ_LOW             = ");
			sb.append(FSKConfig.FREQ_LOW).append(NLC);
			sb.append("FREQ_HIGH            = ");
			sb.append(FSKConfig.FREQ_HIGH).append(NLC);
			sb.append("BAUD                 = ");
			sb.append(FSKConfig.BAUD).append(NLC);

			sb.append(SEP);
			sb.append("SAMPLE_RATE          = ");
			sb.append(FSKConst.SAMPLE_RATE).append(NLC);
			sb.append("SAMPLE_SIZE          = ");
			sb.append(FSKConst.SAMPLE_SIZE).append(NLC);
			sb.append("SAMPLE_MAX           = ");
			sb.append(FSKConst.SAMPLE_MAX).append(NLC);
			sb.append("NUM_CHANNELS         = ");
			sb.append(FSKConst.NUM_CHANNELS).append(NLC);

			sb.append(SEP);
			sb.append("BITS_PER_CHANNEL     = ");
			sb.append(FSKConst.BITS_PER_CHANNEL).append(NLC);
			sb.append("BYTES_PER_FRAME      = ");
			sb.append(FSKConst.BYTES_PER_FRAME).append(NLC);

			sb.append(SEP);
			sb.append("SAMPLE_DURATION      = ");
			sb.append(FSKConst.SAMPLE_DURATION).append(NLC);

			sb.append(SEP);
			sb.append("WL_HIGH              = ");
			sb.append(FSKConst.WL_HIGH).append(NLC);
			sb.append("WL_LOW               = ");
			sb.append(FSKConst.WL_LOW).append(NLC);
			sb.append("HWL_HIGH             = ");
			sb.append(FSKConst.HWL_HIGH).append(NLC);
			sb.append("HWL_LOW              = ");
			sb.append(FSKConst.HWL_LOW).append(NLC);

			sb.append(SEP);
			sb.append("BIT_PERIOD           = ");
			sb.append(FSKConst.BIT_PERIOD).append(NLC);

			sb.append(SEP);
			sb.append("SINE_TABLE_LENGTH    = ");
			sb.append(FSKConst.SINE_TB_LEN).append(NLC);

			sb.append(SEP);
			sb.append("PRE_CARRIER_BITS     = ");
			sb.append(FSKConst.PRE_CARRIER_BITS).append(NLC);
			sb.append("POST_CARRIER_BITS    = ");
			sb.append(FSKConst.POST_CARRIER_BITS).append(NLC);

			sb.append(SEP);
			sb.append("TABLE_JUMP_HIGH      = ");
			sb.append(FSKConst.TABLE_JUMP_HIGH).append(NLC);
			sb.append("TABLE_JUMP_LOW       = ");
			sb.append(FSKConst.TABLE_JUMP_LOW).append(NLC);

			if (FSKConfig.logLevel >= FSKLog.DEBUG) {
				sb.append(SEP);
				sb.append("SINE_TABLE :").append(NLC);
				fskm.debugPrintSineTable(sb);
			}

			sb.append(SEP);

			FSKLog.log(FSKLog.INFO, sb.toString());
		}
	}

	// ////////////////////////////////////////////////////
	// Utility Functions for converting between SAMPLES and NS/US/MS
	// ////////////////////////////////////////////////////

	// #define SAMPLES_TO_NS(__samples__) \
	// (((UInt64)(__samples__) * 1000000000) / SAMPLE_RATE)
	static long SAMPLES_TO_NS(int samples) {
		return (long) ((samples * 1000000000.0) / FSKConst.SAMPLE_RATE);
	}

	// #define NS_TO_SAMPLES(__nanosec__) \
	// (unsigned)(((UInt64)(__nanosec__) * SAMPLE_RATE) / 1000000000)
	static long NS_TO_SAMPLES(int nsec) {
		return (long) ((nsec * FSKConst.SAMPLE_RATE) / 1000000000.0);
	}

	// #define US_TO_SAMPLES(__microsec__) \
	// (unsigned)(((UInt64)(__microsec__) * SAMPLE_RATE) / 1000000)
	static long US_TO_SAMPLES(int usec) {
		return (long) ((usec * FSKConst.SAMPLE_RATE) / 1000000.0);
	}

	// #define MS_TO_SAMPLES(__millisec__) \
	// (unsigned)(((UInt64)(__millisec__) * SAMPLE_RATE) / 1000)
	static long MS_TO_SAMPLES(int msec) {
		return (long) ((msec * FSKConst.SAMPLE_RATE) / 1000.0);
	}

}
