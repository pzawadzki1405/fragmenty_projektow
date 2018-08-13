package com.rster;



import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;


import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Handler;
import android.os.Message;
import android.support.v4.content.LocalBroadcastManager;
import android.widget.TextView;



public class BtConnect {
	
	private final BluetoothDevice device;
	  public static BluetoothSocket socket;
	  private WatekOdczyt readerThread;

	  private static OutputStream outStream;	//strumien wyjscia
	  private static InputStream inStream;
	  private ConnectionF connectF;

	    public static final int MESSAGE_READ = 1;
	    
	    
	    public BtConnect(		//konstruktor
	    		BluetoothDevice device, BluetoothSocket socket, Handler odczytHandler)
	    throws IOException{	//przechwyc IOException
	    	
	    	this.device=device;
	    	BtConnect.socket=socket;
	    	
	    	this.readerThread=new WatekOdczyt(socket.getInputStream(), odczytHandler);
	    	BtConnect.outStream=socket.getOutputStream();
	    	BtConnect.inStream=socket.getInputStream();
	    	
	    	this.readerThread.start();    	
	    }
	    public static void disconnect(){
	    	
	    	if (inStream != null) {
                try {inStream.close();} catch (Exception e) {}
                inStream = null;
	    	}
	    	
        if (outStream != null) {
                try {outStream.close();} catch (Exception e) {}
                outStream = null;
        }

        if (socket != null) {
                try {socket.close();} catch (Exception e) {}
                socket = null;
        }
	    }
	    
	    public void write(byte[] bufor) throws IOException{	//metoda do wysylania
	    	final OutputStream out;
	    	out=BtConnect.outStream;
	    	outStream.write(bufor);
	    }
	    
		public void writec(char c) throws IOException {
			final OutputStream out;
	    	out=BtConnect.outStream;
	    	outStream.write(c);
			
		}
	    
	    public static BtConnect.ConnectionF nowyHandlerBT(BtConnect.ConnectionF obiekt, Handler newHandler) throws IOException{
	    	BtConnect.ConnectionF nowy=obiekt; 
	    	
	    	//nowy.readHandler=newHandler;
	    
	    	
				
					//nowy.odczyt=new WatekOdczyt(socket.getInputStream(), newHandler);
				//	nowy.odczyt.start();
			
				
	    		
			
	    		
	    	//	obiekt.odczyt.notify();
	    	return nowy;
				
				
	    		//zwroc new watek odczyt	
	    		    	
	    	
	    }
	    
	    public static void Alert(String tytul, String wiadomosc, Context kontekst){
			
			final AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(kontekst);
			
				// set title
				alertDialogBuilder.setTitle(tytul);

				// set dialog message
				alertDialogBuilder
					.setMessage(wiadomosc)
					.setCancelable(false)
					.setNeutralButton("OK",new DialogInterface.OnClickListener() {
						public void onClick(DialogInterface dialog,int id) {
							// if this button is clicked, close
							// current activity
							dialog.cancel();
						}
					  });
					

					// create alert dialog
					AlertDialog alertDialog = alertDialogBuilder.create();

					// show it
					alertDialog.show();
	}

	
//////////////////////////	
	public static class ConnectionF {	//klasa pochodna zawierajaca parametry polaczcenia
		public final BluetoothDevice device;
		private final BluetoothAdapter adapter;
		public final Handler readHandler;
	
		
		private BtConnect result;
		private boolean done;
		private boolean failed;
		private WatekPolaczenia thread;

		public BluetoothSocket socket;
		
		//konstruktor;
		public ConnectionF(BluetoothDevice device, Handler readHandler){
			
			this.device=device;
					
			this.readHandler=readHandler;
			this.adapter=BluetoothAdapter.getDefaultAdapter();
			
			this.done=false;
			this.failed=false;	
			
			
			
			this.thread=new WatekPolaczenia(device, adapter, this);
			this.thread.start();
			//this.odczyt=BtConnect.readerThread;
			this.socket=BtConnect.socket;
		}
		

		
		public void rozlacz(){
			disconnect();
		}
		
		public BtConnect get(){	//zwroc result(obiekt BtConnect), zablokuj inne watki
			block();
			return result;
		}
		
		public void block(){
			while(done==false){
				try{
					synchronized(thread){	//zablokowanie innych watkow
						thread.wait();
					}
				}
				catch(InterruptedException e){
					//sprawdz stan jeszcze raz i moze czekaj dalej
				}
			}
		}
		
		public boolean done(){	//utworzenie metod dla atrybutow done i failed
			return done;
		}
		public boolean failed(){
			return failed;
		}

		public void socketZalacz(BluetoothSocket socket) throws IOException{	//zalacz socket, throws przechwyc bledy
			this.failed=false;
			this.result=new BtConnect(device, socket, readHandler
					);
			this.done=true;
		}
		public void socketFailed(){			//nieudane polaczenie
			this.failed=true;
			this.done=true;
		}
		
	}//koniec klasy ConnectionF
////////////////////////////////////////////////	
	private static class WatekPolaczenia extends Thread {		// klasa pochodna watek polaczenia
		private static final UUID MOJE_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
		
		private final BluetoothDevice device;
		private final BluetoothAdapter adapter;
		private final ConnectionF ConnectionF;
		
		public WatekPolaczenia(BluetoothDevice device,	//konstruktor
				BluetoothAdapter adapter,
				ConnectionF ConnectionF){
			this.device=device;
			this.adapter=adapter;
			this.ConnectionF=ConnectionF;		
		}
		
		public void run(){	//polacz
			adapter.cancelDiscovery();	//wylacz wykrywanie
			BluetoothSocket socket;
			
			try{
				socket=device.createRfcommSocketToServiceRecord(MOJE_UUID);
				socket.connect();		//polacz
				ConnectionF.socketZalacz(socket);
				
			} catch (IOException e){
				ConnectionF.socketFailed();
			}
			
			synchronized(this){
				notifyAll();
			}
		}
	}	//koniec watek polaczenie
///////////////////////////////////////////	
	public class WatekOdczyt extends Thread{	//klasa pochodna watek odczytu
		private final InputStream StrumienWejscia; //strumien wejscia
		public final Handler odczytHandler; //handler do odczytu
	
		
		public WatekOdczyt(InputStream StrumienWejscia,
				Handler odczytHandler){
			this.odczytHandler=odczytHandler;
			this.StrumienWejscia=StrumienWejscia;			
		}
		
		public synchronized void run(){
		//	setName("watekodczyt"); //ustaw nazwe watku
			
			byte []bufor = new byte[1024]; //bufor danych
			int liczBufor; //zmienna do zliczania bufora		
		
	
			while(true){
				try{
				liczBufor=StrumienWejscia.read(bufor);
				
				if(odczytHandler != null){	//jesli odczytHandler nie jest pusty to wyslij BUFOR z parametrem MESSAGE_READ
					
					odczytHandler.obtainMessage(MESSAGE_READ, liczBufor, -1, bufor).sendToTarget();

				}
				}catch(IOException e){
					//przechwyc blad
				}
				
			}
			
			
		}
		
		
	} //koniec watek odczyt

}
