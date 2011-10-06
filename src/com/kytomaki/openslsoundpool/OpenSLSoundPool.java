package com.kytomaki.openslsoundpool;

import java.io.FileDescriptor;

import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.util.Log;

/**
 * @author Janne Kytomaki
 *
 */
public class OpenSLSoundPool implements SoundPoolIf {
	public static final int RATE_44_1 = 1;
	public static final int FORMAT_16 = 1;
	
	private static String TAG = "OpenSLSoundPool";
	static public boolean available = false;
	
	static {
		try{
			System.loadLibrary("opensl-soundpool");
			available = true;
		}catch(UnsatisfiedLinkError e){
			Log.e(TAG, e.getMessage());
		}
	}
	
	public OpenSLSoundPool(int maxStreams, int srcSampleRate, int srcBits, int srcChannels){
		if (srcSampleRate != RATE_44_1){
			Log.d(TAG, "Unsupported rate");
			System.exit(1);
		}
		if (srcBits != FORMAT_16){
			Log.d(TAG, "Unsupported bits");
			System.exit(1);
		}
		if (srcChannels != 1){
			Log.d(TAG, "Unsupported channels");
			System.exit(1);
		}
		createEngine(maxStreams);
	}
	
	/* (non-Javadoc)
	 * @see com.kytomaki.OpenSLSoundPool.SoundPoolIf#play(int, float)
	 */
	@Override
	public int play(int soundId, float volume){
		return nativePlay(soundId, volume);
	}
	
	/* (non-Javadoc)
	 * @see com.kytomaki.OpenSLSoundPool.SoundPoolIf#load(android.content.Context, int)
	 */
	@Override
	public int load (Context context, int resId){
     	// try loading a resource into the native code
         AssetFileDescriptor afd =
        		 context.getResources().openRawResourceFd(resId);
 		int res = 0;
 		if (afd != null){
 			FileDescriptor fd = afd.getFileDescriptor();
 			int off = (int) afd.getStartOffset();
 			int len = (int) afd.getLength();
 			res = load(fd, off, len);
 		}
 		return res;
    }
	
	
	
	@Override
	public void release() {
		nativeRelease();
	}

	private static native void createEngine(int maxStreams);
	private static native int load(FileDescriptor fd, int offset, int length);
	private static native int nativePlay(int soundId, float volume);
	private static native int nativeRelease();
	
}
