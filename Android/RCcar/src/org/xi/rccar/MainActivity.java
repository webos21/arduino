package org.xi.rccar;

import org.xi.audiofsk.FSKModem;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends Activity {

	private TCPComm _conn;
	private FSKModem _fskm;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		_conn = new TCPComm();
		_fskm = new FSKModem();

		final Button btnConn = (Button) findViewById(R.id.btnConnect);
		btnConn.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View arg0) {
				EditText txtIp = (EditText) findViewById(R.id.edtIPAddr);
				String ipaddr = txtIp.getText().toString();
				if (!"".equals(ipaddr) && ipaddr.length() > 7) {
					_conn.setDataReceiver(new TCPCommListener() {
						@Override
						public void dataArrived(final int c) {
							_fskm.writeBytes(new byte[] { (byte) c });
							runOnUiThread(new Runnable() {
								@Override
								public void run() {
									TextView recvMsg = (TextView) findViewById(R.id.txtReceived);
									String msg = recvMsg.getText().toString();
									recvMsg.setText(msg + (char) c);
								}
							});
						}
					});
					int result = _conn.connect(ipaddr, 7777);
					Log.i("RCCar", "Connection Result = " + result);
					if (result < 0) {
						Toast toast = Toast.makeText(getApplicationContext(),
								"Connection Failed!!!!", Toast.LENGTH_SHORT);
						toast.show();
					} else {
						if (_conn.isConnected()) {

						}
					}
				}
			}
		});

		final Button btnClear = (Button) findViewById(R.id.btnClear);
		btnClear.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View arg0) {
				TextView recvMsg = (TextView) findViewById(R.id.txtReceived);
				recvMsg.setText(null);
			}
		});

		_fskm.start();
	}

	@Override
	protected void onDestroy() {
		_conn.close();
		_conn = null;

		_fskm.stop();
		_fskm = null;

		super.onDestroy();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.activity_main, menu);
		return true;
	}

}
