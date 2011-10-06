package com.kytomaki.openslsoundpool;

import android.content.Context;
import android.media.AudioManager;
import android.media.SoundPool;

/**
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
