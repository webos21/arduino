package org.xi.audiofsk;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.AudioTrack;
import android.media.MediaRecorder;

/**
 * Android has no call-back functions for audio, but iOS has. So, the
 * implementation of Android simulates the iOS mechanism.
 */
class AndroidAudio {

	// Most Android devices support 'CD-quality' sampling frequencies.
	private static final int _sampleRate = FSKConfig.SAMPLE_RATE;

	// IO is FSK-modulated at either 613 or 1226 Hz (0 / 1)
	private static final int _ioBaseFrequency = 613;

	// For performance reasons we batch update
	// the audio output buffer with this many bits.
	private static final int _bitsInBuffer = 100;

	// Android internal buffer size.
	private static final int _bufferSizeInBytes = _sampleRate;

	// For Playing {
	private AudioTrack _audioTrack;
	private short[] _playBuffer;
	private Runnable _playProcess;
	// }

	// For Recording {
	private AudioRecord _audioRecord;
	private short[] _recBuffer;
	private Runnable _recProcess;
	// }

	// Reference of FSKModem
	private FSKDataInterface _fskdi;

	// Thread Control Flag {
	private boolean _isRunning;
	private Thread _playThread;
	private Thread _recThread;

	// }

	AndroidAudio() {
		_playProcess = new Runnable() {
			public void run() {
				Thread.currentThread().setPriority(Thread.NORM_PRIORITY);
				while (_isRunning) {
					if (_fskdi != null) {
						_fskdi.feedPlayData(_playBuffer, _playBuffer.length);
					}
					// _audioTrack.setStereoVolume(1.0f, 1.0f);
					_audioTrack.write(_playBuffer, 0, _playBuffer.length);
				}
			}
		};
		_recProcess = new Runnable() {
			public void run() {
				int readLen = 0;
				Thread.currentThread().setPriority(Thread.MIN_PRIORITY);
				while (_isRunning) {
					readLen = _audioRecord.read(_recBuffer, 0,
							_recBuffer.length);
					if (_fskdi != null) {
						_fskdi.pullRecData(_recBuffer, readLen);
					}
				}
			}
		};
	}

	private void requestDevice() {
		int bufferSize = (_sampleRate / _ioBaseFrequency / 2);

		// The stereo buffer should be large enough to ensure
		// that scheduling doesn't mess it up.
		_playBuffer = new short[bufferSize * _bitsInBuffer];

		// Open Audio-Player
		_audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, _sampleRate,
				AudioFormat.CHANNEL_CONFIGURATION_MONO,
				AudioFormat.ENCODING_PCM_16BIT, _bufferSizeInBytes,
				AudioTrack.MODE_STREAM);

		int recBufferSize = AudioRecord.getMinBufferSize(_sampleRate,
				AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT);

		_recBuffer = new short[recBufferSize * 10];

		// Open Audio-Recorder
		_audioRecord = new AudioRecord(MediaRecorder.AudioSource.DEFAULT,
				_sampleRate, AudioFormat.CHANNEL_IN_MONO,
				AudioFormat.ENCODING_PCM_16BIT, recBufferSize);

	}

	private void releaseDevice() {
		_audioTrack.release();
		_audioTrack = null;

		_audioRecord.release();
		_audioRecord = null;

		_playBuffer = null;
		_recBuffer = null;
	}

	void setFSKDataInterface(FSKDataInterface fskdi) {
		_fskdi = fskdi;
	}

	void startIO() {
		if (_isRunning || _playThread != null || _recThread != null) {
			FSKLog.log(FSKLog.WARN, "[AndroidAudio] is already working!!!");
			return;
		}

		requestDevice();

		_audioRecord.startRecording();
		_audioTrack.play();

		_isRunning = true;

		_playThread = new Thread(_playProcess);
		_playThread.start();

		_recThread = new Thread(_recProcess);
		_recThread.start();
	}

	void stopIO() {
		_audioRecord.stop();
		_audioTrack.stop();

		_isRunning = false;

		try {
			_playThread.join();
			_recThread.join();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}

		_playThread = null;
		_recThread = null;

		releaseDevice();
	}
}
