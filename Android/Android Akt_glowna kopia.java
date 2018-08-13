package com.rster;





import java.io.IOException;



import android.app.Activity;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Color;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.provider.SyncStateContract.Constants;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.TextView;
import android.widget.Toast;

public class Akt_glowna extends Activity {

	public static String EXTRA_DEVICE_ADDRESS="device adress";	
	private static final int REQUEST_CONNECT_DEVICE = 1;
	private final static int REQUEST_ENABLE_BT = 2;
	public boolean poloczone; 
	public static boolean wygaszenie;
	private Context kontekst = this;
	private BluetoothAdapter BTadapter;
	private Button Wlacz, Wyszukaj, menu;
	private  TextView Urz_nazwa;
	private BluetoothSocket socket;	
	private CheckBox keepscreen;
	public static BtConnect.ConnectionF connectionF=null;
	boolean zaznaczony_menu;
	 //private BtConnect.ConnectionF connectionF=null;
	
	public static TextView displayedTextBox;
	
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.akt_glowna);		
		wygaszenie=true;
		BTadapter = BluetoothAdapter.getDefaultAdapter();
		poloczone = false;
		Wlacz= (Button) findViewById(R.id.Wlacz_g);
		Wyszukaj = (Button) findViewById(R.id.Wyszukaj_g);
		menu= (Button) findViewById(R.id.menu_g);
		Urz_nazwa = (TextView) findViewById(R.id.adres_g);
		zaznaczony_menu=false;
	    // If the adapter is null, then Bluetooth is not supported
/*	    if (bluetoothAdapter == null) {
	      finish();
	      return;
	    }*/
		
		displayedTextBox = (TextView) findViewById(R.id.odbior_g);
		displayedTextBox.setMovementMethod(new ScrollingMovementMethod());    
		
	    
	    if (BTadapter.isEnabled()){
	    	Wlacz.setBackgroundColor(Color.RED);
		    Wlacz.invalidate();
	    	Wlacz.setText("Wylacz BT");
	    }
	    else 	{
	    	Wlacz.setBackgroundColor(Color.GREEN);
		    Wlacz.invalidate();
	    	Wlacz.setText("Wlacz BT");
	    }
		
		Wlacz.setOnClickListener(onWlacz);	
	}
	
	@Override
	public void onPause(){
		super.onPause();
		//handler_test.removeMessages(BtConnect.MESSAGE_READ);
		if (poloczone == true){
		try {
			Akt_glowna.connectionF.get().writec('k');
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		}
	}
	
	@Override
	public void onResume(){
		super.onResume();
		
		if (poloczone == true){
/*		try {
			Akt_glowna.connectionF.get().writec('g');
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}*/
		}
	}
	
	
	private OnClickListener onWlacz = new OnClickListener() {
		 
		@Override
	       public void onClick(View v) {
			
			 
			if (BTadapter.isEnabled()){
				BTadapter.disable();
				Wlacz.setBackgroundColor(Color.GREEN);
			    Wlacz.invalidate();
				Wlacz.setText("Wlacz BT");
				Urz_nazwa.setText("");
				Urz_nazwa.setHint("Rozlaczony");
				
			}
			else {
					// Uzyskiwanie od u¿ytkownika zgody na w³¹czenie Bluetootha
					Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE); 
					startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
					
			}
				
	       }
	 };
	 
	 public void WUrzadz(View target) {				 
		 if (poloczone==true){			
			connectionF.rozlacz();		
			 Wyszukaj.setText("Wyszukaj urzadzenia");
			 poloczone=false;
			 menu.setEnabled(false); 
			 Urz_nazwa.setText("");
			Urz_nazwa.setHint("Rozlaczony");
		 }
		 else{
		 	if (!BTadapter.isEnabled()) BtConnect.Alert("Blad", "BT jest wylaczony",this);
		 	else{
		 		Intent serverintent = new Intent(this,Akt_urzadzenia.class);		
		 		startActivityForResult(serverintent, REQUEST_CONNECT_DEVICE);
		 	}
		 }
		    //finish();
			
			//startActivityForResult(new Intent(BluetoothAdapter.ACTION_REQUEST_DISCOVERABLE), DISCOVERY_REQUEST_BLUETOOTH);
	    }
	 
	 
	 public void Pmenu(View target) {		
		 Intent intent = new Intent(this,Akt_menu.class);	
		 
		 try {
 			Akt_glowna.connectionF.get().writec('k');
 		} catch (IOException e) {
 			// TODO Auto-generated catch block
 			e.printStackTrace();
 		}

		 startActivity(intent);
	    }
	 
	 public void onActivityResult(int requestCode, int resultCode, Intent data) {		    
		    switch (requestCode) {
		    case REQUEST_CONNECT_DEVICE:
		      onSelectDeviceActivityResult(resultCode, data);
		      break;
		    case REQUEST_ENABLE_BT:
		      onEnableBluetoothActivityResult(resultCode, data);
		      break;
		    }
		  }
	 
	 private void onEnableBluetoothActivityResult(int resultCode, Intent data) {
		
		 if (resultCode == Activity.RESULT_OK) {
				String address = BTadapter.getAddress();
				String name = BTadapter.getName();
				 String toastText = name + " : " + address;
				 Toast.makeText(this, toastText, Toast.LENGTH_LONG).show();
				 Wlacz.setBackgroundColor(Color.RED);
				 Wlacz.invalidate();
				 Wlacz.setText("Wylacz BT");
		    } 
	}
	 private int group1Id = 1;

	 int homeId = Menu.FIRST;
	 int profileId = Menu.FIRST +1;
	 int searchId = Menu.FIRST +2;
	 int dealsId = Menu.FIRST +3;
	 int helpId = Menu.FIRST +4;
	 int contactusId = Menu.FIRST +5;
	 	 

	 @Override
	 public boolean onCreateOptionsMenu(Menu menu) {
		
	     menu.add(group1Id, homeId, homeId, "Wylacz Wygaszanie");		
	     menu.add(group1Id, profileId, profileId, "Wlacz Wygaszanie");
	     menu.add(group1Id, searchId, searchId, "");
	     menu.add(group1Id, dealsId, dealsId, "");
	     menu.add(group1Id, helpId, helpId, "");
	     menu.add(group1Id, contactusId, contactusId, "");	
	     return super.onCreateOptionsMenu(menu); 
	 }

	 @Override
	 public boolean onOptionsItemSelected(MenuItem item) {
	     switch (item.getItemId()) {
	     case 1:	    		 
	    	 getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
	    	 wygaszenie=false;
	         return true;

	     case 2:
	    	 getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
	    	 wygaszenie=true;
	         return true;

	     case 3:
	         // write your code here
	         return true;

	     case 4:
	         // write your code here
	         return true;

	     case 5:
	         // write your code here
	         return true;

	     case 6:
	         // write your code here
	         return true;

	     default:
	         return super.onOptionsItemSelected(item);
	     }
	 }
	 
	 private void onSelectDeviceActivityResult(int resultCode, Intent data) {
		 		 
		    if (resultCode == Activity.RESULT_OK) {
		      
		    	String address = data.getExtras()
		        .getString(Akt_urzadzenia.EXTRA_DEVICE_ADDRESS);
		      
		      BluetoothDevice device = BTadapter.getRemoteDevice(address);
		      connectionF = new BtConnect.ConnectionF(device, handler_test);
		      String name = device.getName();
		      String adres = device.getAddress();
		      Urz_nazwa.setText(name+" adres:"+adres);
		     
		      
		      if (connectionF.failed()) {
		    	  Log.i("sss","Connection failed");
		        } else {
		        	Toast.makeText(this, "Laczenie...", Toast.LENGTH_LONG).show();
		          final BtConnect.ConnectionF localConnection = connectionF;
		      //    final Button localButton = transmitButton;
		          Log.i("BlueDuino", "Starting AsyncTask");
		          new AsyncTask<Integer, Integer, Boolean> () {
		              public Boolean doInBackground(Integer... params) {
		                localConnection.block();
		                Log.i("BlueDuino", "done blocking for connection");
		                return localConnection.failed();
		              }

		              public void onPostExecute(Boolean failed) {
		                if (!failed) {		                
		                	poloczone=true;
		                	Wyszukaj.setText("Rozlacz");
		                	menu.setEnabled(true); 
		                	
		                }
		              }
		          }.execute();
		        }
		    }
	 }
	
	 
	    public Handler handler_test = new Handler() {
	    	
		        @Override
		        public synchronized void handleMessage(Message msg) {
		          switch (msg.what) {
		              case BtConnect.MESSAGE_READ:
		            	  
		            	 
		            	  
		                  byte[] readBuf = (byte[]) msg.obj;
		                  String readMessage = new String(readBuf, 0, msg.arg1);		                  
			           /*   if (readMessage.length()>2){
		                  switch(readMessage.charAt(0)){
		                  
		                  }
			              }*/
		                  TextView displayedTextBox = (TextView) findViewById(R.id.odbior_g);
		                  
		                  Intent intent = new Intent("action.odbior");
		            	  intent.putExtra("odbior", readMessage);
		            	  sendBroadcast(intent);
			                
			                displayedTextBox.append(readMessage);

			                final int scrollAmount = displayedTextBox.getLayout().getLineTop(
			                  displayedTextBox.getLineCount()) - displayedTextBox.getHeight();

			              
			                if (scrollAmount > 0) {
			                  displayedTextBox.scrollTo(0, scrollAmount);
			                }
		          }
		        }
		      };
		


	
}
