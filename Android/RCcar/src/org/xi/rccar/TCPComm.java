package org.xi.rccar;

import java.io.IOException;
import java.io.InputStream;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketTimeoutException;

public class TCPComm {
	private Socket _sock;

	private TCPCommListener _lsn;

	private Runnable _recvProcess;

	private Thread _recvThr;

	public TCPComm() {
		_sock = new Socket();
		_recvProcess = new Runnable() {
			@Override
			public void run() {
				try {
					InputStream is = _sock.getInputStream();
					while (_sock.isConnected()) {
						int c = is.read();
						if (_lsn != null && c > 31 && c < 127) {
							_lsn.dataArrived(c);
						}
					}
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		};
	}

	public int connect(String ipaddr, int port) {
		InetSocketAddress saddr = new InetSocketAddress(ipaddr, port);
		try {
			_sock.connect(saddr, 10000);
		} catch (SocketTimeoutException ste) {
			ste.printStackTrace();
			return -2;
		} catch (IOException e) {
			e.printStackTrace();
			return -1;
		}
		_recvThr = new Thread(_recvProcess);
		_recvThr.start();
		return 0;
	}

	public boolean isConnected() {
		return _sock.isConnected();
	}

	public void setDataReceiver(TCPCommListener lsn) {
		_lsn = lsn;
	}

	public void close() {
		try {
			_sock.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
