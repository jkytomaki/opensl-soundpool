package com.kytomaki.openslsoundpool;

import android.content.Context;
import android.media.AudioManager;
import android.media.SoundPool;

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
public class JavaSoundPool implements SoundPoolIf{

	SoundPool pool;
	
	public JavaSoundPool(int maxStreams){
		pool = new SoundPool(maxStreams, AudioManager.STREAM_MUSIC, 0);
	}
	
	@Override
	public int play(int soundId, float volume) {
		return pool.play(soundId, volume, volume, 0, 0, 1.0f);
	}

	@Override
	public int load(Context context, int resId) {
		return pool.load(context, resId, 1);
	}

	@Override
	public void release() {
		pool.release();
		pool = null;
	}

	
	
}
