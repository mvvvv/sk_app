package net.stereokit.sk_app;

import android.app.NativeActivity;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.content.ClipData;

/**
 * Custom NativeActivity subclass that handles Activity results for sk_app.
 *
 * This is required because the base NativeActivity doesn't forward onActivityResult
 * to native code, which is needed for file picker dialogs.
 *
 * To use this, change your AndroidManifest.xml:
 *   android:name="android.app.NativeActivity"     ->  android:name="net.stereokit.sk_app.SkAppActivity"
 *   android:hasCode="false"                       ->  android:hasCode="true"
 */
public class SkAppActivity extends NativeActivity {
	private static final String TAG = "SkAppActivity";

	// Request code marker for sk_app file dialogs (0x5B00 prefix)
	// Lower byte is the dialog ID (0-255), upper byte is the marker
	private static final int SKA_FILE_DIALOG_MASK = 0x5B00;
	private static final int SKA_FILE_DIALOG_ID_MASK = 0x00FF;

	// Track if we've loaded the native library for JNI callbacks
	private static boolean sNativeLibraryLoaded = false;

	// Native callback methods
	private static native void nativeOnFileDialogResult(int dialogId, String[] uris, boolean cancelled);

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// Load native library before super.onCreate() for JNI method resolution
		// NativeActivity also loads it, but we need to ensure our native methods are resolvable
		if (!sNativeLibraryLoaded) {
			try {
				ActivityInfo ai = getPackageManager().getActivityInfo(
					getIntent().getComponent(), PackageManager.GET_META_DATA);
				if (ai.metaData != null) {
					String libName = ai.metaData.getString("android.app.lib_name");
					if (libName != null) {
						Log.i(TAG, "Loading native library: " + libName);
						System.loadLibrary(libName);
						sNativeLibraryLoaded = true;
					}
				}
			} catch (PackageManager.NameNotFoundException e) {
				Log.e(TAG, "Failed to get activity info: " + e.getMessage());
			} catch (UnsatisfiedLinkError e) {
				// Library might already be loaded by NativeActivity
				Log.d(TAG, "Library load returned: " + e.getMessage());
				sNativeLibraryLoaded = true;
			}
		}

		super.onCreate(savedInstanceState);
		Log.i(TAG, "SkAppActivity created");
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		super.onActivityResult(requestCode, resultCode, data);

		// Check if this is our file dialog request (mask both sides to compare upper bits only)
		if ((requestCode & 0xFF00) != (SKA_FILE_DIALOG_MASK & 0xFF00)) {
			Log.d(TAG, "onActivityResult: not a sk_app request (code=" + requestCode + ")");
			return;
		}

		int dialogId = requestCode & SKA_FILE_DIALOG_ID_MASK;
		Log.i(TAG, "File dialog result: dialogId=" + dialogId + ", resultCode=" + resultCode);

		if (resultCode != RESULT_OK || data == null) {
			// User cancelled
			nativeOnFileDialogResult(dialogId, null, true);
			return;
		}

		// Collect URIs from the result
		String[] uris = null;

		// Check for multiple selection
		ClipData clipData = data.getClipData();
		if (clipData != null && clipData.getItemCount() > 0) {
			// Multiple files selected
			uris = new String[clipData.getItemCount()];
			for (int i = 0; i < clipData.getItemCount(); i++) {
				Uri uri = clipData.getItemAt(i).getUri();
				if (uri != null) {
					uris[i] = uri.toString();
					// Take persistent permission for the URI
					takePersistablePermission(uri, data);
				}
			}
		} else {
			// Single file selected
			Uri uri = data.getData();
			if (uri != null) {
				uris = new String[] { uri.toString() };
				// Take persistent permission for the URI
				takePersistablePermission(uri, data);
			}
		}

		if (uris != null && uris.length > 0) {
			Log.i(TAG, "File dialog returned " + uris.length + " URI(s)");
			nativeOnFileDialogResult(dialogId, uris, false);
		} else {
			Log.w(TAG, "File dialog returned no URIs");
			nativeOnFileDialogResult(dialogId, null, true);
		}
	}

	/**
	 * Take persistable URI permission if available.
	 * This allows the app to access the file across reboots.
	 */
	private void takePersistablePermission(Uri uri, Intent data) {
		try {
			int flags = data.getFlags() &
				(Intent.FLAG_GRANT_READ_URI_PERMISSION | Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
			if (flags != 0) {
				getContentResolver().takePersistableUriPermission(uri, flags);
			}
		} catch (SecurityException e) {
			// Permission not available for this URI, that's okay
			Log.d(TAG, "Could not take persistable permission for " + uri + ": " + e.getMessage());
		}
	}
}
