package com.kytomaki.openslsoundpool;

import android.content.Context;

/**
 * @author Janne Kytomaki
 *
 */
public interface SoundPoolIf {

	public abstract int play(int soundId, float volume);

	public abstract int load(Context context, int resId);
	
	public abstract void release();

}