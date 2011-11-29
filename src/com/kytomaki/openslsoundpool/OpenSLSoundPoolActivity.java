package com.kytomaki.openslsoundpool;

import android.app.Activity;
import android.media.AudioManager;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.ImageButton;
import android.widget.RadioGroup;
import android.widget.ToggleButton;

/**
 * Copyright 2011, Janne Kytömäki

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
   
 * @author Janne Kytomaki
 *
 */
public class OpenSLSoundPoolActivity extends Activity {
	
	private static String TAG = "OpenSLSoundPool";
	
	private SoundPoolIf currentPool;
	
	private int[] currentSounds;
	
	private final static int MAX_STREAMS = 24,
						MAX_PLAYER_GAP = 20,
						INT_PLAYER_THREADS = 4;
	
	private boolean playing = false;
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
    	Log.d(TAG, "onCreate");
        super.onCreate(savedInstanceState);
        
        this.setVolumeControlStream(AudioManager.STREAM_MUSIC);
        
        // use Android SoundPool (via a wrapper) by default
        currentPool = new JavaSoundPool(MAX_STREAMS);
        currentSounds = loadSounds(currentPool);
        
        // A thread to play random sounds, enabled and disabled 
        // from a ToggleButton
		
        for (int i = 0; i < INT_PLAYER_THREADS; i++){
        	createPlayerThread();
        }
        
        View view = (View)getLayoutInflater().inflate(R.layout.main, null);
        
        // Button to play a single random sound
        ImageButton b = (ImageButton)view.findViewById(R.id.button1);
        b.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View arg0) {
				playRandomSound();
			}
		});
        
        // ToggleButton to toggle playback of random sounds 
        ToggleButton playBtn = (ToggleButton)view.findViewById(R.id.toggleButton1);
        playBtn.setOnCheckedChangeListener(new ToggleButton.OnCheckedChangeListener() {
			
			@Override
			public void onCheckedChanged(CompoundButton arg0, boolean arg1) {
				playing=arg1;
				
			}
		});
        
        RadioGroup rg = (RadioGroup)view.findViewById(R.id.radioGroup1);
        rg.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
			
			@Override
			public void onCheckedChanged(RadioGroup arg0, int arg1) {
				synchronized(OpenSLSoundPoolActivity.this){
					if (arg1 == R.id.radio_ndk_sp){
						currentPool.release();
						currentPool = new OpenSLSoundPool(MAX_STREAMS, OpenSLSoundPool.RATE_44_1, OpenSLSoundPool.FORMAT_16, 1);
						currentSounds = loadSounds(currentPool);
					}else if (arg1 == R.id.radio_android_sp){
						currentPool.release();
						currentPool = new JavaSoundPool(MAX_STREAMS);
						currentSounds = loadSounds(currentPool);
					}
				}
			}
		});
        
      // if OpenSLSoundPool can't be loaded (<2.3 device), disable
      // the option
      if (OpenSLSoundPool.available){
	    	Log.d(TAG, "OpenSLSoundPool is available");
	    }else{
	    	Log.d(TAG, "OpenSLSoundPool is not available");
	    	rg.findViewById(R.id.radio_ndk_sp).setEnabled(false);
      }
        
      	view.setKeepScreenOn(true);
        setContentView(view);
        
    }

    private void createPlayerThread(){
    	Runnable player = new Runnable() {
			
			@Override
			public void run() {
				boolean running = true;
				while(running){
					if (playing){
						synchronized(OpenSLSoundPoolActivity.this){
							playRandomSound();
						}
					}
					// Sleep between 0 and MAX_PLAYER_GAP ms between
					// playing samples
					try {
						Thread.sleep((int)(Math.random() * MAX_PLAYER_GAP));
					} catch (InterruptedException e) {
						running = false;
					}
				}
			}
		};
        
		Thread t = new Thread(player);
		t.start();
    }

	@Override
	protected void onPause() {
		Log.d(TAG, "onPause");
		playing = false;
		currentPool.release();
		currentPool = null;
		super.onPause();
	}
	
	private int[] loadSounds(SoundPoolIf sp){
		int ids[] = new int[6];
		ids[0] = sp.load(this, R.raw.die1);
		ids[1] = sp.load(this, R.raw.die2);
		ids[2] = sp.load(this, R.raw.die3);
		ids[3] = sp.load(this, R.raw.die_soft);
		ids[4] = sp.load(this, R.raw.die_soft1);
		ids[5] = sp.load(this, R.raw.die_soft2);
		
		return ids;
	}
	
	public void playRandomSound(){
		currentPool.play(currentSounds[(int)(Math.random() * currentSounds.length)], (float)( Math.random() * 1.0f ));
	}
	
    
}