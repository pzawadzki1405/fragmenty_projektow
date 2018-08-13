package com.rster;
import java.io.IOException;
import java.util.List;
import java.util.Set;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Color;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.AdapterView.OnItemClickListener;

public class Akcelerometr extends Activity {
  // Zmienna do zwrocenia wartosci adresu
  public static String EXTRA_DEVICE_ADDRESS = "device_address";

  private Sensor sensor;
  private TextView azimuth, roll, pitch, result, wyslij, max;
  private View prost_l, prost_p, prost_g, prost_d;
  private Button start;
  private SensorManager sensorManager;
  private Integer maxG, maxD, maxP, maxL;
  
  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    // Ustawienie okienka
   
    setContentView(R.layout.akcelerometr);
    
    if (Akt_glowna.wygaszenie){
    	getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }else{
    	getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }
    
    sensorManager = (SensorManager)getSystemService(SENSOR_SERVICE);
   
    azimuth = (TextView) findViewById(R.id.azi_a);
    roll = (TextView) findViewById(R.id.pitch_a);
    pitch = (TextView) findViewById(R.id.roll_a);
    result = (TextView) findViewById(R.id.result_a);
    wyslij = (TextView) findViewById(R.id.wyslij_t_a);
    max = (TextView) findViewById(R.id.max_t_a);
    
    prost_l = (View) findViewById(R.id.prost_lewo);      
    prost_p = (View) findViewById(R.id.prost_prawo);       
    prost_d = (View) findViewById(R.id.prost_dol);       
    prost_g = (View) findViewById(R.id.prost_gora);   
    zmienkolor(null);
    
    start = (Button) findViewById(R.id.Start_ster);
    start.setOnTouchListener(startDotyk);
    
    
	if(Nastawy.akc==true){
		maxG = Nastawy.maxG;
		maxG = Math.round((maxG*4/10));
		maxP = Nastawy.maxP;
		maxP = Math.round((maxP*4/10));
		maxD = Nastawy.maxD;
		maxD = Math.round((maxD*4/10));
		maxL = Nastawy.maxL;
		maxL = Math.round((maxL*4/10));
	}else{
		maxG = 30;
		maxD = 30;
		maxP = 45;
		maxL = 45;
	}
    
	max.setText("maxG=" + maxG.toString() + 
			" maxD=" + maxD.toString() + 
			" maxP=" + maxP.toString() + 
			" maxL=" + maxL.toString());
    
    List<android.hardware.Sensor> sensorList=
    		sensorManager.getSensorList(Sensor.TYPE_ORIENTATION);
    
    if (sensorList.size()>0){
    	sensor = sensorList.get(0);
    	
    }
    else{
    	
    }
    try {
		Akt_glowna.connectionF.get().writec('a');
	} catch (IOException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
	}
    
    
  //  sensorManager.registerListener(orientationListener, sensor, 0, null);
    
  }
  
  @Override
	public void onPause(){
		super.onPause();
		
		try {
			Akt_glowna.connectionF.get().writec('k');
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}		
	}
	
	@Override
	public void onResume(){
		super.onResume();
		
		try {
			Akt_glowna.connectionF.get().writec('a');
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}	
		
	}
  
  private OnTouchListener startDotyk = new OnTouchListener() {
		 
      @Override
      public boolean onTouch(View v, MotionEvent event) {
          if (event.getAction() == MotionEvent.ACTION_DOWN ) {
        	  sensorManager.registerListener(orientationListener, sensor, 0, null);
        	  wyslij.setText("wysylanie");
              return true;
          } else if (event.getAction() == MotionEvent.ACTION_UP){
        	  sensorManager.unregisterListener(orientationListener);
        	  wyslij.setText("stop wysylanie");
        	  zmienkolor(null);
        	  
        	  try {
					Akt_glowna.connectionF.get().writec('s');
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
        	  
        	  return true;
          }

          return false;
      }
	 };
  
  
  private SensorEventListener orientationListener = new SensorEventListener(){
	  
	  @Override
	  public void onAccuracyChanged(Sensor arg0, int arg1){
		  
	  }
	  
	  public void onSensorChanged (SensorEvent sensorEvent){
		  if(sensorEvent.sensor.getType()==Sensor.TYPE_ORIENTATION){
			  float azimuth_v = sensorEvent.values[0];
			  float pitch_v = sensorEvent.values[1];
			  float roll_v = sensorEvent.values[2];

			  
			  
			  azimuth.setText(String.format("%.2f", azimuth_v));
			  pitch.setText(String.format("%.2f", pitch_v));
			  roll.setText(String.format("%.2f", roll_v));
			  
			  
			  
			  if (pitch_v <-maxD && pitch_v > -90){
				  result.setText("Dol");
				  zmienkolor(prost_d);				 
				  
					try {
						Akt_glowna.connectionF.get().writec('d');
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				
			  } else if (pitch_v > maxG && pitch_v <90){
				  result.setText("Gora");
				  zmienkolor(prost_g);
				  
				  try {
						Akt_glowna.connectionF.get().writec('g');
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				  
			  } else if (roll_v > maxL){
				  result.setText("Lewo");
				  zmienkolor(prost_l);
				  try {
						Akt_glowna.connectionF.get().writec('l');
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
			  } else if (roll_v <-maxP){
				  result.setText("Prawo");
				  zmienkolor(prost_p);
				  
				  try {
						Akt_glowna.connectionF.get().writec('p');
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				  
			  } else {
				  result.setText("NIC");
				  
				  try {
						Akt_glowna.connectionF.get().writec('s');
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				  
				  zmienkolor(null);
				  
				  
			  }
			  
		  }
	  }
	  
	  
  };
  
  private void zmienkolor (View prost){
	  prost_p.setBackgroundColor(Color.RED);
	  prost_l.setBackgroundColor(Color.RED);
	  prost_d.setBackgroundColor(Color.RED);
	  prost_g.setBackgroundColor(Color.RED);
	  
	  if (prost != null) prost.setBackgroundColor(Color.GREEN);
  }
  
}