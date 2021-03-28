/*
 * Copyright (C) 2015 The CyanogenMod Project
 *               2017-2019 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.lineageos.settings.refresh_rate;

import android.app.ActionBar;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.DialogFragment;
import android.content.Context;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Handler;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.preference.Preference;
import androidx.preference.PreferenceCategory;
import androidx.preference.PreferenceFragment;
import androidx.preference.ListPreference;

public class RefreshRateFragment extends PreferenceFragment {

    private TextView mTextView;

    private ListPreference mPrefRefreshRate;

    private Handler mHandler = new Handler();

    @Override
    public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
        addPreferencesFromResource(R.xml.refresh_rate_settings);

        final ActionBar actionBar = getActivity().getActionBar();
        actionBar.setDisplayHomeAsUpEnabled(true);

        SharedPreferences prefs = getActivity().getSharedPreferences("refresh_rate_settings", Activity.MODE_PRIVATE);

        mPrefRefreshRate = (ListPreference) findPreference( "pref_refresh_rate" );
        mPrefRefreshRate.setOnPreferenceChangeListener(PrefListener);
    }

    @Override
    public void onResume() {
        super.onResume();
        mPrefRefreshRate.setValue(Integer.toString(Utils.getRefreshRate(getActivity())));
        mPrefRefreshRate.setSummary(mPrefRefreshRate.getEntry());
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        final View view = LayoutInflater.from(getContext()).inflate(R.layout.refresh_rate, container, false);
        ((ViewGroup) view).addView(super.onCreateView(inflater, container, savedInstanceState));
        return view;
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
    }

    private final Preference.OnPreferenceChangeListener PrefListener =
            new Preference.OnPreferenceChangeListener() {
                @Override
                public boolean onPreferenceChange(Preference preference, Object value) {

                        SharedPreferences sharedPref = getActivity().getSharedPreferences("pref_refresh_rate", Context.MODE_PRIVATE);
                        SharedPreferences.Editor editor = sharedPref.edit();
                        editor.putInt("refresh_rate", Integer.parseInt((String) value) );
                        editor.commit();

                        int refreshRateIndex = mPrefRefreshRate.findIndexOfValue((String) value);
			mPrefRefreshRate.setSummary(mPrefRefreshRate.getEntries()[refreshRateIndex]);

                        Utils.setRefreshRate(Integer.parseInt((String) value));
                    return true;
            }
	};
}
