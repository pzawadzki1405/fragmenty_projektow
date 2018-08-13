package com.rster;

import java.io.IOException;
import java.util.Timer;
import java.util.TimerTask;

import com.rster.Akt_wyslij.MyReceiver;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.os.Bundle;
import android.text.InputFilter;
import android.text.Spanned;
import android.util.SparseArray;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnKeyListener;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.view.ViewGroup.LayoutParams;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.TextView.OnEditorActionListener;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.Spinner;

public class Nastawy extends Activity {
	View prost_1, prost_2;
	SeekBar seekPWM, seekAL, seekAP, seekAD, seekAG;
	EditText editPWM, editAL, editAP, editAD, editAG;
	LinearLayout linpwm1, linpwm2, linakc1, linakc2, linakc3, linakc4, linakc5,linwl1,linwl2;
	Spinner spiner;	
	SparseArray<EditText> array = new SparseArray<EditText>();
	Integer liczbawl;
	Context appcontext;
	static Integer maxG, maxD, maxP, maxL;
	static boolean akc;
	private boolean zawiera;
	private MyReceiver myReceiver;
	public static final String ACTION_NEW_MSG = "action.odbior";
    public static final String MSG_FIELD = "odbior";
	
    SharedPreferences shared;
    SharedPreferences.Editor sharededitor;
    
	  @Override
	  public void onCreate(Bundle savedInstanceState) {
	    super.onCreate(savedInstanceState);
	    setContentView(R.layout.nastawy);
	    
	    shared = getSharedPreferences("Nastawy", Context.MODE_PRIVATE);
		sharededitor= shared.edit();
	    
	    if (Akt_glowna.wygaszenie){
	    	getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
	    }else{
	    	getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
	    }
		
	    akc=false;
	    appcontext = getApplicationContext();
	    prost_1 = (View) findViewById(R.id.prost1_n);      
	    prost_2 = (View) findViewById(R.id.prost2_n); 
	    seekPWM = (SeekBar) findViewById(R.id.bar_PWM_n);
	    seekAL = (SeekBar) findViewById(R.id.barl_akc_n);
	    seekAP = (SeekBar) findViewById(R.id.barp_akc_n);
	    seekAD = (SeekBar) findViewById(R.id.bard_akc_n);
	    seekAG = (SeekBar) findViewById(R.id.barg_akc_n);
	    
	    editPWM = (EditText) findViewById(R.id.edit_PWM_n);
	    editAL = (EditText) findViewById(R.id.editl_akc_n);
	    editAP = (EditText) findViewById(R.id.editp_akc_n);
	    editAD = (EditText) findViewById(R.id.editd_akc_n);
	    editAG = (EditText) findViewById(R.id.editg_akc_n);
	    
	    linpwm1 = (LinearLayout) findViewById(R.id.lin_pwm1_n);	    
	    linpwm2 = (LinearLayout) findViewById(R.id.lin_pwm2_n);
	    linakc1 = (LinearLayout) findViewById(R.id.lin_akc1_n);
	    linakc2 = (LinearLayout) findViewById(R.id.lin_akc2_n);
	    linakc3 = (LinearLayout) findViewById(R.id.lin_akc3_n);
	    linakc4 = (LinearLayout) findViewById(R.id.lin_akc4_n);
	    linakc5 = (LinearLayout) findViewById(R.id.lin_akc5_n);
	    linwl1 = (LinearLayout) findViewById(R.id.lin_wl1_n);
	    linwl2 = (LinearLayout) findViewById(R.id.lin_wl2_n);
	    
	    liczbawl=1;
	    
	    spiner= (Spinner) findViewById(R.id.spinner1_n);//fetch the spinner from layout file
	    ArrayAdapter<String> adapter = new ArrayAdapter<String>(this,
	                android.R.layout.simple_spinner_item, getResources()
	                        .getStringArray(R.array.nastawy));//setting the country_array to spinner
	    adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
	    spiner.setAdapter(adapter);
	  
	    spiner.setOnItemSelectedListener(spinerlistener);
	    
	    prost_1.setBackgroundColor(Color.RED);
	    prost_2.setBackgroundColor(Color.RED);
	    initReceiver();
	    editPWM.setFilters(new InputFilter[]{ new InputFilterMinMax("0", "100")});
	    editAG.setFilters(new InputFilter[]{ new InputFilterMinMax("0", "100")});
	    editAP.setFilters(new InputFilter[]{ new InputFilterMinMax("0", "100")});
	    editAL.setFilters(new InputFilter[]{ new InputFilterMinMax("0", "100")});
	    editAD.setFilters(new InputFilter[]{ new InputFilterMinMax("0", "100")});
	    seekPWM.setMax(100);
	    seekAL.setMax(100);
	    seekAP.setMax(100);
	    seekAD.setMax(100);
	    seekAG.setMax(100);
	    
	    
	    try {
			Akt_glowna.connectionF.get().writec('n');
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	    

		
		//Intent.putExtra("wynik", wynikS);
	    
	    
	    seekPWM.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {									
	    	int progres =0;
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {progres = progress;}														
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {}							
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {editPWM.setText(Integer.toString(progres));}						
		});
	    
	    seekAL.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {									
	    	int progres =0;
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {progres = progress;}														
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {}							
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {editAL.setText(Integer.toString(progres));}						
		});
	    
	    seekAP.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {									
	    	int progres =0;
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {progres = progress;}														
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {}							
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {editAP.setText(Integer.toString(progres));}							
		});
	    
	    seekAD.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
	    	int progres =0;
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {progres = progress;}														
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {}							
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {editAD.setText(Integer.toString(progres));}							
		});
	    
	    seekAG.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {									
	    	int progres =0;
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {progres = progress;}														
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {}							
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {editAG.setText(Integer.toString(progres));}							
		});
	    
	    editPWM.setOnKeyListener(new OnKeyListener() {			
			@Override
			public boolean onKey(View v, int keyCode, KeyEvent event) {
				 if ((event.getAction() == KeyEvent.ACTION_DOWN) && (keyCode == KeyEvent.KEYCODE_ENTER)) {
				seekPWM.setProgress(Integer.parseInt(editPWM.getText().toString()));
				 return true;
				 }
				return false;
			}
		});
	    
	    editAP.setOnKeyListener(new OnKeyListener() {			
			@Override
			public boolean onKey(View v, int keyCode, KeyEvent event) {
				 if ((event.getAction() == KeyEvent.ACTION_DOWN) && (keyCode == KeyEvent.KEYCODE_ENTER)) {
				seekAP.setProgress(Integer.parseInt(editAP.getText().toString()));
				 return true;
				 }
				return false;
			}
		});
	    editAL.setOnKeyListener(new OnKeyListener() {			
			@Override
			public boolean onKey(View v, int keyCode, KeyEvent event) {
				 if ((event.getAction() == KeyEvent.ACTION_DOWN) && (keyCode == KeyEvent.KEYCODE_ENTER)) {
				seekAL.setProgress(Integer.parseInt(editAL.getText().toString()));
				 return true;
				 }
				return false;
			}
		});
	    editAD.setOnKeyListener(new OnKeyListener() {			
			@Override
			public boolean onKey(View v, int keyCode, KeyEvent event) {
				 if ((event.getAction() == KeyEvent.ACTION_DOWN) && (keyCode == KeyEvent.KEYCODE_ENTER)) {
				seekAD.setProgress(Integer.parseInt(editAD.getText().toString()));
				 return true;
				 }
				return false;
			}
		});
	    editAG.setOnKeyListener(new OnKeyListener() {			
			@Override
			public boolean onKey(View v, int keyCode, KeyEvent event) {
				 if ((event.getAction() == KeyEvent.ACTION_DOWN) && (keyCode == KeyEvent.KEYCODE_ENTER)) {
				seekAG.setProgress(Integer.parseInt(editAG.getText().toString()));
				 return true;
				 }
				return false;
			}
		});
	    
	  	
	    
	  }
	  
	  @Override
	    protected void onDestroy() {
	        finishReceiver();
	        super.onDestroy();
	    }
		

		
		@Override
		public void onPause(){
			super.onPause();
			//nowyreadHandler.removeCallbacksAndMessages(null);
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
				Akt_glowna.connectionF.get().writec('n');
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
		}
		
		public OnItemSelectedListener spinerlistener = new OnItemSelectedListener()	{

			@Override
			public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
				String item = spiner.getSelectedItem().toString();
				
				if (item.matches("PWM")){
					linpwm1.setVisibility(View.VISIBLE);
					linpwm2.setVisibility(View.VISIBLE);
					linakc1.setVisibility(View.GONE);
					linakc2.setVisibility(View.GONE);
					linakc3.setVisibility(View.GONE);
					linakc4.setVisibility(View.GONE);
					linakc5.setVisibility(View.GONE);
					linwl1.setVisibility(View.GONE);
					linwl2.setVisibility(View.GONE);
				}
				if (item.matches("Akcelerometr")){
					linpwm1.setVisibility(View.GONE);
					linpwm2.setVisibility(View.GONE);
					linakc1.setVisibility(View.VISIBLE);
					linakc2.setVisibility(View.VISIBLE);
					linakc3.setVisibility(View.VISIBLE);
					linakc4.setVisibility(View.VISIBLE);
					linakc5.setVisibility(View.VISIBLE);
					linwl1.setVisibility(View.GONE);
					linwl2.setVisibility(View.GONE);
				}
				if (item.matches("Wlasne")){
					linpwm1.setVisibility(View.GONE);
					linpwm2.setVisibility(View.GONE);
					linakc1.setVisibility(View.GONE);
					linakc2.setVisibility(View.GONE);
					linakc3.setVisibility(View.GONE);
					linakc4.setVisibility(View.GONE);
					linakc5.setVisibility(View.GONE);
					linwl1.setVisibility(View.VISIBLE);
					linwl2.setVisibility(View.VISIBLE);
				}
				
				
			}

			@Override
			public void onNothingSelected(AdapterView<?> parent) {
				linpwm1.setVisibility(View.GONE);
				linpwm2.setVisibility(View.GONE);
				linakc1.setVisibility(View.GONE);
				linakc2.setVisibility(View.GONE);
				linakc3.setVisibility(View.GONE);
				linakc4.setVisibility(View.GONE);
				linakc5.setVisibility(View.GONE);
				linwl1.setVisibility(View.GONE);
				linwl2.setVisibility(View.GONE);
				
			}
			
		};
	  
	  public void Akc(View target) {
		  if (editAD.getText().toString().matches("") ||
				  editAG.getText().toString().matches("") ||
				  editAP.getText().toString().matches("") ||
				  editAL.getText().toString().matches("")  )
			  BtConnect.Alert("Blad", "Jeden lub wiele parametrow jest puste", this);
		  else{
		  akc=true;
		  maxD=Integer.parseInt(editAD.getText().toString());
		  maxG=Integer.parseInt(editAG.getText().toString());
		  maxP=Integer.parseInt(editAP.getText().toString());
		  maxL=Integer.parseInt(editAL.getText().toString());
		  Toast.makeText(this, "Zmieniono parametry akcelerometru", Toast.LENGTH_SHORT).show();
		  }
	  	}
	  
	  public void WlasneDodaj(View target) {		  
		  LinearLayout linear = new LinearLayout(this);
		  EditText nazwazmiennej = new EditText(this);
		  EditText wartosczmiennej = new EditText(this);
		  linear.setLayoutParams(new LayoutParams(LayoutParams.MATCH_PARENT,LayoutParams.WRAP_CONTENT));
		  nazwazmiennej.setLayoutParams(new LayoutParams(220,LayoutParams.WRAP_CONTENT));
		  wartosczmiennej.setLayoutParams(new LayoutParams(220,LayoutParams.WRAP_CONTENT));
		  nazwazmiennej.setHint("Nazwa");
		  nazwazmiennej.setId(liczbawl);		  
		  liczbawl++;
		  array.put(nazwazmiennej.getId(), nazwazmiennej);
		  wartosczmiennej.setHint("Wartosc");
		  wartosczmiennej.setId(liczbawl);		  
		  liczbawl++;
		  array.put(wartosczmiennej.getId(), wartosczmiennej);
		  ((LinearLayout) linear).addView(nazwazmiennej);
		  ((LinearLayout) linear).addView(wartosczmiennej);
		
	        ((LinearLayout) linwl2).addView(linear);

	  	}
	  
	  public void WlasneWyslij(View target) {		  
		  
		  
		  sharededitor.putBoolean("zawiera", true);		  
		  try {
				Akt_glowna.connectionF.get().writec('w');
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} 
		for (int i=0; i<array.size(); i++){
			EditText nowy= array.get(array.keyAt(i));
			try {
				Akt_glowna.connectionF.get().write(nowy.getText().toString().getBytes());
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			i=i+1;
			sharededitor.putString(nowy.getText().toString(), nowy.getText().toString());
			nowy= array.get(array.keyAt(i));
			  try {
					Akt_glowna.connectionF.get().write(nowy.getText().toString().getBytes());
					Akt_glowna.connectionF.get().writec('z');
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
		
		}
		try {
			Akt_glowna.connectionF.get().writec('c');
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		Toast.makeText(this, "Zmieniono wartosci: " +
					(array.size()/2)+ " zmiennych!", Toast.LENGTH_SHORT).show();
		

	  	}
	  

	  
	  public void PWM(View target) {
		  if (editPWM.getText().toString().matches("") )
			  BtConnect.Alert("Blad", "Puste pole", this);
		  else{
			  try {
					Akt_glowna.connectionF.get().writec('p');
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			  try {
					Akt_glowna.connectionF.get().write((editPWM.getText().toString().getBytes()));
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} 
			  try {
					Akt_glowna.connectionF.get().writec('z');
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			  
		  Toast.makeText(this, "Zmieniono PWM", Toast.LENGTH_SHORT).show();
		  }
	  	}
	  
	  public class MyReceiver extends BroadcastReceiver {
	        @Override
	        public void onReceive(Context context, Intent intent) {
	           
	            	if (intent.getAction().equals(ACTION_NEW_MSG)) {
	                    String message = intent.getStringExtra(MSG_FIELD);
	                    
/*	                    if (message.matches("z")){
	                    	 Toast.makeText(appcontext, message, Toast.LENGTH_SHORT).show();
	                    }*/
	                    
	                }
					
				
	        }
	    };
	    
	    private void initReceiver() {
	        myReceiver = new MyReceiver();
	        IntentFilter filter = new IntentFilter(ACTION_NEW_MSG);
	        registerReceiver(myReceiver, filter);
	    }
	    
	    private void finishReceiver() {
	        unregisterReceiver(myReceiver);
	    }
	    
	    
	    public class InputFilterMinMax implements InputFilter {

	        private int min, max;

	        public InputFilterMinMax(int min, int max) {
	            this.min = min;
	            this.max = max;
	        }

	        public InputFilterMinMax(String min, String max) {
	            this.min = Integer.parseInt(min);
	            this.max = Integer.parseInt(max);
	        }

	        @Override
	        public CharSequence filter(CharSequence source, int start, int end, Spanned dest, int dstart, int dend) {   
	            try {
	                int input = Integer.parseInt(dest.toString() + source.toString());
	                if (isInRange(min, max, input))
	                    return null;
	            } catch (NumberFormatException nfe) { }     
	            return "";
	        }

	        private boolean isInRange(int a, int b, int c) {
	            return b > a ? c >= a && c <= b : c >= b && c <= a;
	        }
	    }
	  
}
