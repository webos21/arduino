package org.xi.audiofsk;

/**
 * Receive the byte-data from recorded audio
 */
public interface FSKModemListener {
	public void dataReceivedFromFSKModem(byte[] data);
}
