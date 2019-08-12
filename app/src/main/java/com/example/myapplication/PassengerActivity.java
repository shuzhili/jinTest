package com.example.myapplication;

import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.AutoCompleteTextView;
import android.widget.Button;
import android.widget.TextView;

import com.amap.api.location.AMapLocation;
import com.amap.api.location.AMapLocationClient;
import com.amap.api.location.AMapLocationClientOption;
import com.amap.api.location.AMapLocationListener;
import com.amap.api.maps.AMap;
import com.amap.api.maps.CameraUpdateFactory;
import com.amap.api.maps.MapView;
import com.amap.api.maps.model.BitmapDescriptorFactory;
import com.amap.api.maps.model.LatLng;
import com.amap.api.maps.model.Marker;
import com.amap.api.maps.model.MarkerOptions;
import com.amap.api.maps.overlay.DrivingRouteOverlay;
import com.amap.api.services.core.LatLonPoint;
import com.amap.api.services.core.PoiItem;
import com.amap.api.services.help.Inputtips;
import com.amap.api.services.help.InputtipsQuery;
import com.amap.api.services.help.Tip;
import com.amap.api.services.poisearch.PoiResult;
import com.amap.api.services.poisearch.PoiSearch;
import com.amap.api.services.route.BusRouteResult;
import com.amap.api.services.route.DrivePath;
import com.amap.api.services.route.DriveRouteResult;
import com.amap.api.services.route.RideRouteResult;
import com.amap.api.services.route.RouteSearch;
import com.amap.api.services.route.WalkRouteResult;

import java.util.ArrayList;
import java.util.List;

public class PassengerActivity extends AppCompatActivity {

    private MapView _mapView = null;
    private AMap _amap = null;
    private AMapLocationClient _amapLocationClient = null;
    private AMapLocationClientOption _amapLocationOption = null;
    private Button _bt_startOrder = null;
    private TextView _tv_srcAddr = null;
    private AutoCompleteTextView _attv_dstAddr = null;

    private Marker _selfMarker = null;
    private boolean isAddSelfMarker = false;

    private LatLonPoint _startPoint = null;
    private LatLonPoint _endPoint = null;
    private String _city="";
    protected  void initUI() {
        _mapView = (MapView)findViewById(R.id.PassengerMap);
        _bt_startOrder = (Button)findViewById(R.id.bt_startOrder);
        _tv_srcAddr = (TextView)findViewById(R.id.tv_passenger_srcAddr);
        _attv_dstAddr = (AutoCompleteTextView)findViewById(R.id.attv_passenger_dstAddr);

    }

    //以某个经纬度为中心来展示地图
    protected void moveMap(double latitude, double longtiude)
    {
        LatLng lagLng = new LatLng(latitude, longtiude);

        //移动amap地图 以之前的缩放比例展示
        _amap.animateCamera(CameraUpdateFactory.newLatLngZoom(lagLng, _amap.getCameraPosition().zoom));
    }

    //向固定的经纬度添加一个标记
    protected void addMarkerToMap(double latitude, double longitude)
    {
        _selfMarker = _amap.addMarker(new MarkerOptions().position(new LatLng(latitude, longitude))
                .icon(BitmapDescriptorFactory.fromBitmap(BitmapFactory.decodeResource(getResources(), R.drawable.location_marker))));
    }


    protected void createMap(Bundle savedInstanceState) {
        //展示地图容器
        _mapView.onCreate(savedInstanceState);
        //得到amap对象
        _amap = _mapView.getMap();
        //默认显示实时交通信息
        _amap.setTrafficEnabled(true);

    }

    //启动定位服务器
    protected  void doLocation()
    {
        //1 创建一个客户端定位句柄
        _amapLocationClient = new AMapLocationClient(getApplicationContext());
        //1.5 给定位客户端设置一些属性
        _amapLocationOption = new AMapLocationClientOption();
        //每个5s定位一次
        _amapLocationOption.setInterval(3000);
        //_amapLocationOption.setOnceLocation(true);

        //将option设置给client对象
        _amapLocationClient.setLocationOption(_amapLocationOption);

        //2 给客户端句柄设置一个listenner来处理服务器返回的定位数据
        _amapLocationClient.setLocationListener(new AMapLocationListener() {
            @Override
            public void onLocationChanged(AMapLocation aMapLocation) {
                //onLocationChanged 就是如果服务器给客户端返回数据，调用的回调函数
                //aMapLocation 就是服务器给客户端返回的定位数据
                if (aMapLocation != null) {
                    //服务器是有响应的
                    if(aMapLocation.getErrorCode() == 0) {
                        //定位成功，aMapLocation获取数据
                        Log.e("Amap", "location succ address = "+ aMapLocation.getAddress());
                        Log.e("Amap", "city = "+ aMapLocation.getCity());
                        Log.e("Amap", "longtitude = " + aMapLocation.getLongitude());
                        Log.e("Amap", "latitude = " + aMapLocation.getLatitude());
                        _city=aMapLocation.getCity();
                        if (isAddSelfMarker == false) {
                            //在此位置添加一个标记
                            addMarkerToMap(aMapLocation.getLatitude(), aMapLocation.getLongitude());
                            isAddSelfMarker = true;

                            //以自我为中心展示地图
                            moveMap(aMapLocation.getLatitude(), aMapLocation.getLongitude());
                        }

                        if (_startPoint == null) {
                            //得到乘客的起始坐标点
                            _startPoint = new LatLonPoint(aMapLocation.getLatitude(),aMapLocation.getLongitude());
                        }

                        //设置乘客源地址信息
                        _tv_srcAddr.setText(aMapLocation.getAddress());
                    }
                    else {
                        //定位失败，
                        Log.e("Amap", "location error, code = "+ aMapLocation.getErrorCode()+
                                ", info = "+ aMapLocation.getErrorInfo());
                    }
                }
            }
        });

        //3 开启定位服务
        _amapLocationClient.startLocation();
    }

    protected void doSearchPOI(){
        _bt_startOrder.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //开始搜索POI兴趣点
                Log.e("Amap","button onclick");
                String dstAddr=_attv_dstAddr.getText().toString();
                //开始POI搜索
                //创建一个搜索的条件对象query
                PoiSearch.Query query=new PoiSearch.Query(dstAddr,"",_city);
                //创建POISearch句柄和query关联
                PoiSearch poiSearch=new PoiSearch(getApplicationContext(),query);
                //给search一个回调函数
                poiSearch.setOnPoiSearchListener(new PoiSearch.OnPoiSearchListener() {
                    @Override
                    public void onPoiSearched(PoiResult poiResult, int i) {
                        if(i!=1000){
                            Log.e("Amap","poi search error code="+i);
                            return;
                        }
                        List<PoiItem> poiList=poiResult.getPois();
                        for(int index=0;index<poiList.size();index++){
                            //此时表示处理每个已经搜索到的兴趣点
                            Log.e("Amap","搜索到的兴趣点有");
                            PoiItem item=poiList.get(index);
                            Log.e("Amap","poi title="+item.getTitle()+"latitude="+
                                    item.getLatLonPoint().getLatitude()+"longtitude="+item.getLatLonPoint().getLongitude());
                            //给每个搜索到的标记点画一个标记
                            addMarkerToMap(item.getLatLonPoint().getLatitude(),item.getLatLonPoint().getLongitude());
                            //默认以第一个兴趣点为我们的坐标点
                            _endPoint=new LatLonPoint(item.getLatLonPoint().getLatitude(),
                                    item.getLatLonPoint().getLongitude());

                            drawRouteLine();
                            if(index==0){
                                break;
                            }
                        }
                    }

                    @Override
                    public void onPoiItemSearched(PoiItem poiItem, int i) {

                    }
                });
                //启动搜索
                poiSearch.searchPOIAsyn();
            }
        });
    }
    //绘制驾驶交通路径
    private void drawRouteLine(){
        //创建路径的绘制句柄routeSearch
        RouteSearch routeSearch=new RouteSearch(getApplicationContext());
        RouteSearch.FromAndTo ft=new RouteSearch.FromAndTo(_startPoint,_endPoint);
        //设置一个路径搜索的query
        RouteSearch.DriveRouteQuery query=new RouteSearch.DriveRouteQuery(ft,RouteSearch.DrivingDefault,null,null,"");
        //给绘制路径的句柄设置一个callback函数
        routeSearch.setRouteSearchListener(new RouteSearch.OnRouteSearchListener() {
            @Override
            public void onBusRouteSearched(BusRouteResult busRouteResult, int i) {
                //公交处理业务
            }

            @Override
            public void onDriveRouteSearched(DriveRouteResult driveRouteResult, int i) {
                //驾驶处理业务
                if(i!=1000){
                    Log.e("Amap","搜索驾驶路径失败");
                    return ;
                }
                //画出驾驶路径
                //拿到得到的路径
                DrivePath path=driveRouteResult.getPaths().get(0);
                //驾驶路径一个覆盖物
                DrivingRouteOverlay drivingRouteOverlay=new DrivingRouteOverlay(
                        getApplicationContext(),
                        _amap,
                        path,
                        _startPoint,
                        _endPoint
                );
                //把之前的路径删除掉
                _amap.clear();
                //以适当的缩放显示路径
                drivingRouteOverlay.zoomToSpan();
                //去掉中间转弯的一些图标提示
                drivingRouteOverlay.setNodeIconVisibility(false);
                drivingRouteOverlay.setThroughPointIconVisibility(false);

                //将路径添加到地图
                drivingRouteOverlay.addToMap();
            }

            @Override
            public void onWalkRouteSearched(WalkRouteResult walkRouteResult, int i) {

            }

            @Override
            public void onRideRouteSearched(RideRouteResult rideRouteResult, int i) {

            }
        });

        //3 启动路径所有 将query穿进去 向服务器发送请求
        routeSearch.calculateDriveRouteAsyn(query);
    }

    //绑定attv控件 自动搜索高德地图的兴趣点列表 自动弹出输入提示列表
    protected  void initAutoCompeleteTextView()
    {
        //给attv控件设置一个阈值
        _attv_dstAddr.setThreshold(1);

        //给autocompleteTextView绑定自动补齐功能
        _attv_dstAddr.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {

            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
                //当文本内容发生改变的时候 调用此回调函数


                //从高德的服务器获取有关跟关键字匹配的poi名称的数据

                //(1) 得到要搜索的关键字
                final String keyword = _attv_dstAddr.getText().toString();
                if(keyword == null || keyword.length() ==0) {
                    Log.e("Amap", "search keyword == null");
                    return;
                }

                //(2) 创建一个query 查询所有tips的条件
                InputtipsQuery query = new InputtipsQuery(keyword, _city);

                //(3) 创建一个InputTips 查询句柄
                Inputtips search = new Inputtips(getApplicationContext(), query);

                //(4) 给InputTips 设定回调函数
                search.setInputtipsListener(new Inputtips.InputtipsListener() {
                    @Override
                    public void onGetInputtips(List<Tip> list, int i) {

                        if (i != 1000) {
                            Log.e("Amap", "search input tips error i = "+ i);
                            return;
                        }


                        // 1 应该从服务器获取能够匹配的 单词集合
                        ArrayList<String> poiList = new ArrayList<String>();

                        /*
                        poiList.add("gailun");
                        poiList.add("gailun1");
                        poiList.add("gailun2");
                        poiList.add("gailun3");
                        poiList.add("gailun4");
                        poiList.add("gailun5");
                        */

                        for (int index = 0; index < list.size(); index++) {
                            Log.e("Amap", "通过 "+keyword +"匹配到的tips 有 "+list.get(index).getName());
                            poiList.add(list.get(index).getName());

                        }


                        // 2 给autoCompleteTextView 设置一个适配器Adapter
                        ArrayAdapter<String> adapter = new ArrayAdapter<String>(getApplicationContext(),
                                android.R.layout.simple_list_item_1, poiList);

                        // 3 将Adapter  和 给autoCompleteTextView 相关联
                        _attv_dstAddr.setAdapter(adapter);

                        // 4 触发adapter 触发控件显示  单词集合
                        adapter.notifyDataSetChanged();

                    }
                });


                //(5) 开启InputTips 向服务器发送查询请求
                search.requestInputtipsAsyn();


            }

            @Override
            public void afterTextChanged(Editable s) {

            }
        });
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_passenger);
        initUI();
        createMap(savedInstanceState);
        doLocation();

        doSearchPOI();
        initAutoCompeleteTextView();
    }
}
