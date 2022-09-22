package com.nooskewl.dogo;

import java.io.File;
import java.io.FileFilter;
import java.util.Locale;
import java.nio.file.Path;
import java.nio.file.PathMatcher;
import java.nio.file.FileSystems;
import java.util.Vector;

import android.os.Bundle;
import android.os.Build;
import android.content.Context;
import android.content.Intent;
import android.graphics.Canvas;
import android.graphics.Color;
import android.net.Uri;
import android.os.Vibrator;
import android.os.VibrationEffect;
import android.util.Log;
import android.view.Surface;

import org.libsdl.app.SDLActivity; 

import com.nooskewl.dogo.License_Viewer_Activity;

/*
import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.GooglePlayServicesUtil;
import com.google.android.gms.common.api.GoogleApiClient;
import com.google.android.gms.common.api.GoogleApiClient.ConnectionCallbacks;
import com.google.android.gms.common.api.GoogleApiClient.OnConnectionFailedListener;
import com.google.android.gms.games.Games;
import com.google.android.gms.games.achievement.Achievements;
import android.app.Dialog;
import android.support.v4.app.DialogFragment;
import android.content.DialogInterface;
import android.content.IntentSender.SendIntentException;
*/

public class DogO_Activity extends SDLActivity
{
	final static int LICENSE_REQUEST = 1;
	final static int MANUAL_REQUEST = 1;

	native static void resume_after_showing_license();
	native static void resume_after_showing_manual();
	native static void resume_after_showing_achievements();
	native static void pause();
	native static void resume();

	// This is so the screen is never cleared pure black, only shim::black (r:35, g:30, b:60)
	static boolean paused = false;

	//private static final int RC_ACHIEVEMENT_UI = 9002;

	@Override
	public void onCreate(Bundle savedInstance)
	{
		super.onCreate(savedInstance);
		
		//mResolvingError = savedInstance != null && savedInstance.getBoolean(STATE_RESOLVING_ERROR, false);
	}

	@Override
	public void onActivityResult(int requestCode, int resultCode, Intent data)
	{
		if (requestCode == LICENSE_REQUEST) {
			if (data != null) {
				if (resultCode == RESULT_OK && data.getExtras().getString("MESSAGE").equals("OK")) {
					show_license_result = 0;
				}
				else if (resultCode == RESULT_CANCELED && data.getExtras().getString("MESSAGE").equals("FAIL")) {
					show_license_result = 1;
				}
				else {
					show_license_result = 1;
				}
			}
			else {
				show_license_result = 1;
			}

			resume_after_showing_license();
		}
		else if (requestCode == MANUAL_REQUEST) {
			if (data != null) {
				if (resultCode == RESULT_OK && data.getExtras().getString("MESSAGE").equals("OK")) {
					show_manual_result = 0;
				}
				else if (resultCode == RESULT_CANCELED && data.getExtras().getString("MESSAGE").equals("FAIL")) {
					show_manual_result = 1;
				}
				else {
					show_manual_result = 1;
				}
			}
			else {
				show_manual_result = 1;
			}

			resume_after_showing_manual();
		}
		/*
		else if (requestCode == RC_ACHIEVEMENT_UI) {
			resume_after_showing_achievements();
		}
		*/
		/*
		else if (requestCode == REQUEST_RESOLVE_ERROR) {
			mResolvingError = false;
			if (resultCode == RESULT_OK) {
				// Make sure the app is not already connected or attempting to connect
				if (!api_client.isConnecting() && !api_client.isConnected()) {
					Log.d("Dog-O", "still not connected...");
					api_client.connect();
				}
			}
			else {
				Log.d("Dog-O", "FAIL! Result code = " + Integer.toString(resultCode));
			}
		}
		*/
	}

	public void onStart() {
		super.onStart();
/*
		if (play_services_available) {
			api_client.connect();
		}
		*/
	}

	@Override
	public void onStop()
	{
		super.onStop();
		/*
		if (play_services_available) {
			api_client.disconnect();
		}
		*/
		pause();
	}
	
	@Override
	public void onRestart()
	{
		super.onRestart();
		resume();
	}

	@Override
	public void onResume()
	{
		super.onResume();
	}

	@Override
	public void onPostResume()
	{
		super.onPostResume();
		paused = true;
	}

	public void logString(String s)
	{
		Log.d("Dog-O", s);
	}

	public String getAppdataDir()
	{
		return getFilesDir().getAbsolutePath();
	}
	
	public String getSDCardDir()
	{
		File f = getExternalFilesDir(null);
		if (f != null) {
			return f.getAbsolutePath();
		}
		else {
			return getFilesDir().getAbsolutePath();
		}
	}

	static int show_license_result;

	public void showLicense()
	{
		show_license_result = -1;
		Intent intent = new Intent(this, License_Viewer_Activity.class);
		startActivityForResult(intent, LICENSE_REQUEST);
	}

	static int show_manual_result;

	public void showManual(String language)
	{
		if (language.equals("french")) {
			show_manual_result = -1;
			Intent intent = new Intent(this, French_Manual_Viewer_Activity.class);
			startActivityForResult(intent, MANUAL_REQUEST);
		}
		else {
			show_manual_result = -1;
			Intent intent = new Intent(this, English_Manual_Viewer_Activity.class);
			startActivityForResult(intent, MANUAL_REQUEST);
		}
	}

	public int getShowLicenseResult()
	{
		return show_license_result;
	}

	public int getShowManualResult()
	{
		return show_manual_result;
	}

	/*
	public void openURL(String url)
	{
		Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
		startActivity(intent);
	}
	*/

	public void rumble(int milliseconds)
	{
		Vibrator v = (Vibrator)getSystemService(Context.VIBRATOR_SERVICE);
		if (v != null && v.hasVibrator()) {
			if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
				v.vibrate(VibrationEffect.createOneShot(milliseconds, VibrationEffect.DEFAULT_AMPLITUDE));
			}
			else {
				v.vibrate(milliseconds);
			}
		}
	}

	public boolean has_touchscreen()
	{
		return getPackageManager().hasSystemFeature("android.hardware.touchscreen");
	}

	public boolean has_vibrator()
	{
		Vibrator v = (Vibrator)getSystemService(Context.VIBRATOR_SERVICE);
		if (v != null) {
			return v.hasVibrator();
		}
		else {
			return false;
		}
	}

	public void start_draw()
	{
		if (paused) {
			paused = false;
		}
	}
	
	public String get_android_language()
	{
		return Locale.getDefault().getLanguage();
	}

	private static File[] list_dir_files = null;

	public void list_dir_start(String path)
	{
		try {
			int slash = path.lastIndexOf('/');
			final String glob = path.substring(slash+1).replace("*", ".*"); // +1 works even if not found (-1+1 == 0)
			String dir = path.substring(0, slash);
			File f = new File(dir);
			list_dir_files = f.listFiles(new FileFilter() {
				public boolean accept(File f)
				{
					try {
						if (f.getName().matches(glob)) {
							return true;
						}
						else {
							return false;
						}
					}
					catch (Exception e) {
						Log.d("Dog-O", "list_dir_start FileFilter throwing " + e.getMessage());
						return false;
					}
				}
			});
		}
		catch (Exception e) {
			list_dir_files = null;
			Log.d("Dog-O", "list_dir_start throwing " + e.getMessage());
		}
	}

	public String list_dir_next()
	{
		if (list_dir_files == null) {
			return "";
		}
		else if (list_dir_files.length == 0) {
			list_dir_files = null;
			return "";
		}
		else {
			File f = list_dir_files[0];
			String name = f.getName();
			if (list_dir_files.length == 1) {
				list_dir_files = null;
			}
			else {
				File[] new_list = new File[list_dir_files.length-1];
				for (int i = 1; i < list_dir_files.length; i++) {
					new_list[i-1] = list_dir_files[i];
				}
				list_dir_files = new_list;
			}
			return name;
		}
	}

	private static final String ARC_DEVICE_PATTERN = ".+_cheets|cheets_.+";

	public boolean is_chromebook()
	{
		// Google uses this, so should work?
		return Build.DEVICE != null && Build.DEVICE.matches(ARC_DEVICE_PATTERN);
	}
	
/*
	public boolean show_achievements()
	{
		if (api_client == null) {
			Log.d("Dog-O", "api_client == null");
			return false;
		}

		if (api_client.isConnected() == false) {
			Log.d("Dog-O", "api_client.isConnected() == false");
			return false;
		}

		play_services_available = check_play_services_status();

		if (play_services_available) {
			Log.d("Dog-O", "Starting achievements UI");
			startActivityForResult(Games.Achievements.getAchievementsIntent(api_client), RC_ACHIEVEMENT_UI);
			return true;
		}
		else {
			Log.d("Dog-O", "Play Services is NOT available");
			return false;
		}
	}
		*/

	//boolean play_services_available;
	//GoogleApiClient api_client;
	// Request code to use when launching the resolution activity
	//private static final int REQUEST_RESOLVE_ERROR = 1001;
	// Unique tag for the error dialog fragment
	//private static final String DIALOG_ERROR = "dialog_error";
	// Bool to track whether the app is already resolving an error
	//private boolean mResolvingError = false;

/*
	public void onConnectionFailed(ConnectionResult result) {
		play_services_available = false;
		Log.d("Dog-O", "Connection failed with result = " + Integer.toString(result.getErrorCode()));
		if (mResolvingError) {
			// Already attempting to resolve an error.
			return;
		}
		else if (result.hasResolution()) {
			try {
				mResolvingError = true;
				result.startResolutionForResult(this, REQUEST_RESOLVE_ERROR);
			}
			catch (SendIntentException e) {
				// There was an error with the resolution intent. Try again.
				api_client.connect();
			}
		}
		else {
			// Show dialog using GooglePlayServicesUtil.getErrorDialog()
			showErrorDialog(result.getErrorCode());
			mResolvingError = true;
		}
	}
	*/

	/* Creates a dialog for an error message */
	
	/*private void showErrorDialog(int errorCode) {
		// Create a fragment for the error dialog
		ErrorDialogFragment dialogFragment = new ErrorDialogFragment();
		// Pass the error that should be displayed
		Bundle args = new Bundle();
		args.putInt(DIALOG_ERROR, errorCode);
		dialogFragment.setArguments(args);
		dialogFragment.show(getSupportFragmentManager(), "errordialog");
	}*/
	
	/* Called from ErrorDialogFragment when the dialog is dismissed. */
	/*public void onDialogDismissed() {
		mResolvingError = false;
	}*/

	/* A fragment to display an error dialog */
	/*public static class ErrorDialogFragment extends DialogFragment {
		public ErrorDialogFragment() { }

		@Override
		public Dialog onCreateDialog(Bundle savedInstance) {
			// Get the error code and retrieve the appropriate dialog
			int errorCode = this.getArguments().getInt(DIALOG_ERROR);
			//return GoogleApiAvailability.getInstance().getErrorDialog(this.getActivity(), errorCode, REQUEST_RESOLVE_ERROR);
			return GooglePlayServicesUtil.getErrorDialog(errorCode, this.getActivity(), REQUEST_RESOLVE_ERROR);
		}

		@Override
		public void onDismiss(DialogInterface dialog) {
			((DogO_Activity)getActivity()).onDialogDismissed();
		}
	}
	
	private static final String STATE_RESOLVING_ERROR = "resolving_error";

	@Override
	public void onSaveInstanceState(Bundle outState) {
		super.onSaveInstanceState(outState);
		outState.putBoolean(STATE_RESOLVING_ERROR, mResolvingError);
	}*/
/*
	public boolean check_play_services_status() {
		int status = GooglePlayServicesUtil.isGooglePlayServicesAvailable(getApplicationContext());

		if (status == ConnectionResult.SUCCESS) {
			return true;
		}
		else {
			Dialog d = GooglePlayServicesUtil.getErrorDialog(status, this, 0);
			d.show();
			return false;
		}
	}

	public void onConnected(Bundle connectionHint) {
		play_services_available = true;
		if (play_services_available && api_client.isConnected()) {
			Games.setViewForPopups(api_client, findViewById(android.R.id.content));
		}
	}

	public void onConnectionSuspended(int cause) {
	}

	public void start_google_play_games_services()
	{
		play_services_available = check_play_services_status();

		if (play_services_available) {
			Log.d("Dog-O", "play services IS available");
			api_client = new GoogleApiClient.Builder(getApplicationContext())
				.addConnectionCallbacks(this)
				.addOnConnectionFailedListener(this)
				.addApi(Games.API)
				.addScope(Games.SCOPE_GAMES)
				.build();

			api_client.connect();
		}
		else {
			Log.d("Dog-O", "play services IS *NOT* available");
		}
	}
	
	public void achieve(String id)
	{
		if (api_client == null || api_client.isConnected() == false) {
			return;
		}

		play_services_available = check_play_services_status();
		if (play_services_available) {
			Achievements.UpdateAchievementResult r = Games.Achievements.unlockImmediate(api_client, id).await();
			if (r.getStatus().isSuccess()) {
				logString("Achievement " + id + " unlocked!");
			}
			else {
				logString("Error unlocking achievement: " + Integer.toString(r.getStatus().getStatusCode()));
			}
		}
		else {
			logString("Play Services not available, can't achieve :(");
		}
	}
	*/
}
