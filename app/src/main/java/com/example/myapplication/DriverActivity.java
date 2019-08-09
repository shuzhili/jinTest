package com.example.myapplication;

import android.graphics.BitmapFactory;
import android.os.Build;
import android.os.Bundle;
import android.os.PersistableBundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;

import com.amap.api.location.AMapLocation;
import com.amap.api.location.AMapLocationClient;
import com.amap.api.location.AMapLocationClientOption;
import com.amap.api.location.AMapLocationListener;
import com.amap.api.maps.AMap;
import com.amap.api.maps.AMapOptions;
import com.amap.api.maps.CameraUpdateFactory;
import com.amap.api.maps.MapView;
import com.amap.api.maps.UiSettings;
import com.amap.api.maps.model.BitmapDescriptorFactory;
import com.amap.api.maps.model.LatLng;
import com.amap.api.maps.model.Marker;
import com.amap.api.maps.model.MarkerOptions;

public class DriverActivity extends AppCompatActivity {

    private MapView _mapView = null;
    private AMap _amap = null;
    private UiSettings _uiSettings = null;

    private AMapLocationClient _amapLocationClient = null;
    private AMapLocationClientOption _amapLoactionOption = null;
    private boolean isAddSelfMarker=false;

    private Marker _selfMarker=null;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_driver);
        _mapView = (MapView) findViewById(R.id.DriverMap);
        createMap(savedInstanceState);
        doLocation();
    }

    private void createMap(Bundle savedInstanceState) {
        _mapView.onCreate(savedInstanceState);
        _amap = _mapView.getMap();
        _amap.setTrafficEnabled(true);
        _uiSettings = _amap.getUiSettings();
        _uiSettings.setCompassEnabled(true);
        _uiSettings.setScaleControlsEnabled(true);
        _uiSettings.setLogoPosition(AMapOptions.LOGO_POSITION_BOTTOM_RIGHT);
    }

    private void doLocation() {
        _amapLocationClient = new AMapLocationClient(getApplicationContext());
        _amapLoactionOption=new AMapLocationClientOption();
        _amapLoactionOption.setInterval(3000);
        //_amapLoactionOption.setOnceLocationLatest(true);
        _amapLocationClient.setLocationOption(_amapLoactionOption);
        _amapLocationClient.setLocationListener(new AMapLocationListener() {
            @Override
            public void onLocationChanged(AMapLocation aMapLocation) {
                if(aMapLocation!=null){
                    if(aMapLocation.getErrorCode()==0){
                        Log.e("Amap","location succ address="+aMapLocation.getAddress());
                        Log.e("Amap","city="+aMapLocation.getCity());
                        Log.e("Amap","longtitude="+aMapLocation.getLongitude());
                        Log.e("Amap","latitude="+aMapLocation.getLatitude());

                        if(isAddSelfMarker==false){
                            addMarkerToMap(aMapLocation.getLatitude(),aMapLocation.getLongitude());
                            isAddSelfMarker=true;
                            moveMap(aMapLocation.getLatitude(),aMapLocation.getLongitude());
                        }
                    }else{
                        Log.e("Amap","location error,code="+aMapLocation.getErrorCode()+
                                "info="+aMapLocation.getErrorInfo());
                    }
                }
            }
        });

        _amapLocationClient.startLocation();
    }

    private void addMarkerToMap(double latitude,double longitude){
        _selfMarker=_amap.addMarker(new MarkerOptions().position(new LatLng(latitude,longitude))
                .icon(BitmapDescriptorFactory.fromBitmap(BitmapFactory.decodeResource(getResources(),R.drawable.location_marker))));
    }

    private void moveMap(double latitude,double longtitude){
        LatLng laglng=new LatLng(latitude,longtitude);
        _amap.animateCamera(CameraUpdateFactory.newLatLngZoom(laglng,_amap.getCameraPosition().zoom));
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        _mapView.onDestroy();
    }

    @Override
    protected void onResume() {
        super.onResume();
        _mapView.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
        _mapView.onPause();
    }

    @Override
    public void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        _mapView.onSaveInstanceState(outState);
    }
}
