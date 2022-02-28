/*
 - 読み込んだファイル( png/jpg or bin ）からマップを生成するクラス
*/
#include"RouteMap.h"
#include<opencv2/opencv.hpp>
#include<queue>
#include<sstream>

#include"Texture2D.h"
#include"Obstacle.h"


#pragma comment(lib, "opencv_world454d.lib") 

using namespace DirectX; 
using json = nlohmann::json; 

const float COL = 100.0f; 
const float ROW = 100.0f; 

json _json;

void readFile(std::string filename, std::vector<uint8_t>& vec)
{
    std::ifstream file(filename, std::ios::binary);
    file.unsetf(std::ios::skipws);

    std::streampos fileSize;
    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    vec.reserve(fileSize);
    vec.insert(vec.begin(),
        std::istream_iterator<uint8_t>(file),
        std::istream_iterator<uint8_t>());
}

// binaryファイルの書き込み
std::string writeMapName(std::string filename)
{
    std::string name = "";
    bool flag = true; 
    for (int i = 0; i < filename.size(); ++i)
    {
        if (flag)
        {
            if (filename[filename.size() - 1 - i] == '.')
            {
                flag = false; 
            }
            continue; 
        }

        if (filename[filename.size() - 1 - i] == '\\')
        {
            break; 
        }

        name = filename[filename.size() - 1 - i] + name; 
    }

    return name; 
}

void writeFile(std::string filename, std::vector<uint8_t>& vec)
{
    std::ofstream file(filename, std::ios::out | std::ios::binary);
    file.write((char*)&vec[0], vec.size() * sizeof(vec[0]));
    file.close();
}

void RouteMap::writeJsonFile(std::string filename)
{
    // JSON関連
    // レコードタイムの更新
    if (_json["recordTime"] < 0.0f)
    {
        _json["recordTime"] = _recordTime;
    }
    else if (_json["recordTime"] > _recordTime)
    {
        _json["recordTime"] = _recordTime;
    }

    std::vector<std::uint8_t> vecj = json::to_msgpack(_json);

    // 保存する名前
    writeFile("mapbin/" + writeMapName(filename) + ".bin", vecj);
}

// binファイルからmap作成
bool RouteMap::CreateMapFromBin(std::string filename)
{
    std::vector<uint8_t> vec; 
    readFile(filename, vec); 
    _json = json::from_msgpack(vec); 

    _recordTime = _json["recordTime"]; 

    /*--床の作成--*/
    for (int y = 0; y < ROW; ++y)
    {
        for (int x = 0; x < COL; ++x)
        {
            float l = float(x) * 150.0f - 7500.0f;
            float r = float(y) * 150.0f - 7500.0f;
            float l2 = float(x + 1) * 150.0f - 7500.0f;
            float r2 = float(y + 1) * 150.0f - 7500.0f;
            XMFLOAT3 p1 = XMFLOAT3(l, 0.0f, r);
            XMFLOAT3 p2 = XMFLOAT3(l2, 0.0f, r);
            XMFLOAT3 p3 = XMFLOAT3(l2, 0.0f, r2);
            XMFLOAT3 p4 = XMFLOAT3(l, 0.0f, r2);
            RectanglePolygon* rect = new RectanglePolygon(p1, p2, p3, p4);

            _mapRects.emplace_back(rect);
            _routeType.emplace_back(_json["routeType"][y * COL + x]); 
        }
    }

    /*--障害物の作成--*/
    XMFLOAT3 p1, p2, p3, p4; 

    for (int i = 0; i < _json["cntObs"]; ++i)
    {
        float minx, maxx, minz, maxz; 
        std::ostringstream oss;
        oss << i;
        std::string num = oss.str();

        minx = _json["obstalce"][num][0]; 
        maxx = _json["obstalce"][num][1]; 
        minz = _json["obstalce"][num][2]; 
        maxz = _json["obstalce"][num][3]; 
        
        // 長方形の用意
        p1 = XMFLOAT3(minx, 130.0f, minz); // 左
        p2 = XMFLOAT3(maxx, 130.0f, minz); // 上
        p3 = XMFLOAT3(maxx, 130.0f, maxz); // 右
        p4 = XMFLOAT3(minx, 130.0f, maxz); // 下

        Obstacle* obs = new Obstacle(p1, p2, p3, p4);

        _obstacles.emplace_back(obs);
    }

    /*--スタート地点の設定--*/
    _startPos.x = _json["startPosx"];
    _startPos.z = _json["startPosz"];
    _startAngle = _json["startAngle"];

    /*-----マテリアルの読み込み開始------*/
    printf("Rectangle-Floor::マテリアルの読み込み開始\n");

    // ダート
    descriptorHeap = new DescriptorHeap();
    auto texPath = ToWideString("Texture/grass.png");
    auto mainTex = Texture2D::Get(texPath);
    descriptorHandle = descriptorHeap->Register(mainTex);

    _descriptorHeaps.emplace_back(descriptorHeap);
    _descriptorHandles.emplace_back(descriptorHandle);

    // 道
    DescriptorHeap* desheap = new DescriptorHeap();
    texPath = ToWideString("Texture/stone.png");
    mainTex = Texture2D::Get(texPath);
    auto deshandle = desheap->Register(mainTex);

    _descriptorHeaps.emplace_back(desheap);
    _descriptorHandles.emplace_back(deshandle);

    // スタート
    DescriptorHeap* desheap2 = new DescriptorHeap();
    texPath = ToWideString("Texture/red.png");
    mainTex = Texture2D::Get(texPath);
    auto deshandle2 = desheap2->Register(mainTex);

    _descriptorHeaps.emplace_back(desheap2);
    _descriptorHandles.emplace_back(deshandle2);

    // 障害物
    DescriptorHeap* desheap3 = new DescriptorHeap();
    texPath = ToWideString("Texture/stone.png");
    mainTex = Texture2D::Get(texPath);
    auto deshandle3 = desheap3->Register(mainTex);

    _descriptorHeaps.emplace_back(desheap3);
    _descriptorHandles.emplace_back(deshandle3);

    // 縁石
    DescriptorHeap* desheap4 = new DescriptorHeap();
    texPath = ToWideString("Texture/enseki.png");
    mainTex = Texture2D::Get(texPath);
    auto deshandle4 = desheap4->Register(mainTex);

    _descriptorHeaps.emplace_back(desheap4);
    _descriptorHandles.emplace_back(deshandle4);
 
    return true; 
}

// pngファイルからmap作成
bool RouteMap::CreateMap(std::string filename)
{
    _recordTime = -1000.0f; 
 
    /*-----マップ画像の読み込み------*/ 
	cv::Mat img = cv::imread(filename, -1); 

	if (img.empty())
	{
		printf("画像の読み込みに失敗しました\n"); 
        return false; 
	}

    cv::resize(img, img, cv::Size(), COL / img.cols, ROW / img.rows); // 縦横100ピクセルに変換

    _mapRects.clear(); 
    std::vector<std::vector<int>> G(img.rows);

    printf("マップ床の生成開始\n"); 
    /*------マップ画像のRGB値からマップを生成する------*/
    for (int y = 0; y < img.rows; ++y)
    {
        cv::Vec3b* ptr = img.ptr<cv::Vec3b>(y); 
        for (int x = 0; x < img.cols; ++x)
        {
            cv::Vec3b bgr = ptr[x];
            
            float l = float(x) * 150.0f - 7500.0f; 
            float r = float(y) * 150.0f - 7500.0f;
            float l2 = float(x + 1) * 150.0f - 7500.0f;
            float r2 = float(y + 1) * 150.0f - 7500.0f;
            XMFLOAT3 p1 = XMFLOAT3(l, 0.0f, r);
            XMFLOAT3 p2 = XMFLOAT3(l2, 0.0f, r);
            XMFLOAT3 p3 = XMFLOAT3(l2, 0.0f, r2);
            XMFLOAT3 p4 = XMFLOAT3(l, 0.0f, r2);
            RectanglePolygon* rect = new RectanglePolygon(p1, p2, p3, p4); 

            _mapRects.emplace_back(rect); 

            // png/jpgファイルの色ごとに属性を分ける
            if (bgr[0] >= 200 && bgr[1] >= 200 && bgr[2] >= 200) // 白
            {
                // ダート
                _routeType.emplace_back(0); 
                G[y].emplace_back(0);
                _json["routeType"].push_back(0);
            }
            else if (bgr[0] <= 50 && bgr[1] >= 200 && bgr[2] <= 50) // 緑
            {
                // スタート地点
                _routeType.emplace_back(2); 
                G[y].emplace_back(2);
                _json["routeType"].push_back(2);
            }
            else if (bgr[0] <= 50 && bgr[1] <= 50 && bgr[2] >= 200) // 赤
            {
                // 障害物ゾーン
                _routeType.emplace_back(3);
                G[y].emplace_back(3);
                _json["routeType"].push_back(3);
            }
            else if (bgr[0] >= 200 && bgr[1] <= 50 && bgr[2] <= 50) // 青
            {
                // アンカー
                _routeType.emplace_back(4); 
                G[y].emplace_back(4); 
                _json["routeType"].push_back(4);
            }
            else
            {
                // 道
                _routeType.emplace_back(1); 
                G[y].emplace_back(1);
                _json["routeType"].push_back(1);
            }

        }
    }

    printf("マップルートの生成完了\n"); 

    /*-----障害物の範囲検出------*/
    std::vector<std::vector<bool>> chk(img.rows);
    std::vector<std::vector<int>> Id(img.rows);

    bool anchor = true; 
    /*------配列の初期化 + アンカーの有無の確認------*/ 
    for (int y = 0; y < img.rows; ++y)
    {
        for (int x = 0; x < img.cols; ++x)
        {
            Id[y].push_back(-1);
            if (G[y][x] == 3) chk[y].push_back(false);
            else chk[y].push_back(true);

            if (G[y][x] == 4) anchor = false; 
        }
    }

    if (anchor)
    {
        printf("アンカーが設定されていません\n"); 
        return false; 
    }

    /*------幅優先探索により範囲検出------*/ 
    int cnt = 0; // 障害物範囲の数
    for (int y = 0; y < img.rows; ++y)
    {
        for (int x = 0; x < img.cols; ++x)
        {
            if (G[y][x] != 3) continue;
            if (chk[y][x]) continue;

            std::queue<std::pair<int, int>> que;
            que.push(std::make_pair(x, y));

            while (!que.empty())
            {
                int _x, _y;
                auto _pair = que.front();
                que.pop();
                _x = _pair.first;
                _y = _pair.second;

                // (x+1, y) 
                if ((_x + 1) < img.cols)
                {
                    // 障害物ならば
                    if (G[_y][_x + 1] == 3)
                    {
                        if (!chk[_y][_x + 1])
                        {
                            Id[_y][_x + 1] = cnt;
                            chk[_y][_x + 1] = true;
                            que.push(std::make_pair(_x + 1, _y));
                        }
                    }
                }

                // (x-1, y) 
                if ((_x - 1) >= 0)
                {
                    // 障害物ならば
                    if (G[_y][_x - 1] == 3)
                    {
                        if (!chk[_y][_x - 1])
                        {
                            Id[_y][_x - 1] = cnt;
                            chk[_y][_x - 1] = true;
                            que.push(std::make_pair(_x - 1, _y));
                        }
                    }
                }

                // (x, y+1) 
                if ((_y + 1) < img.rows)
                {
                    // 障害物ならば
                    if (G[_y + 1][_x] == 3)
                    {
                        if (!chk[_y + 1][_x])
                        {
                            Id[_y + 1][_x] = cnt;
                            chk[_y + 1][_x] = true;
                            que.push(std::make_pair(_x, _y + 1));
                        }
                    }
                }

                // (x, y-1) 
                if ((_y - 1) >= 0)
                {
                    // 障害物ならば
                    if (G[_y - 1][_x] == 3)
                    {
                        if (!chk[_y - 1][_x])
                        {
                            Id[_y - 1][_x] = cnt;
                            chk[_y - 1][_x] = true;
                            que.push(std::make_pair(_x, _y - 1));
                        }
                    }
                }
            }
            ++cnt;
        }
    }

    _json["cntObs"] = cnt; 

    /*-----障害物の作成------*/
    // 検出した範囲に障害物を作成する
    for (int i = 0; i < cnt; ++i)
    {
        XMFLOAT3 p1, p2, p3, p4; 
        float minx, minz, maxx, maxz; 
        bool first_flag = true; 
        for (int y = 0; y < img.rows; ++y)
        {
            for (int x = 0; x < img.cols; ++x)
            {
                if (Id[y][x] != i) continue; 
                float l = float(x) * 150.0f - 7500.0f;
                float r = float(y) * 150.0f - 7500.0f;
                float l2 = float(x + 1) * 150.0f - 7500.0f;
                float r2 = float(y + 1) * 150.0f - 7500.0f;

                if (first_flag)
                {
                    minx = l; 
                    maxx = l2; 
                    minz = r; 
                    maxz = r2; 

                    first_flag = false; 
                    continue; 
                }

                if (minx > l) minx = l; 
                if (maxx < l2) maxx = l2; 
                if (minz > r) minz = r; 
                if (maxz < r2) maxz = r2; 

            }
        }

        // 長方形の用意
        p1 = XMFLOAT3(minx, 130.0f, minz); // 左
        p2 = XMFLOAT3(maxx, 130.0f, minz); // 上
        p3 = XMFLOAT3(maxx, 130.0f, maxz); // 右
        p4 = XMFLOAT3(minx, 130.0f, maxz); // 下
       
        Obstacle* obs = new Obstacle(p1, p2, p3, p4); 

        _obstacles.emplace_back(obs);
        std::ostringstream oss; 
        oss << i; 
        std::string num = oss.str(); 
        _json["obstalce"][num].push_back(minx); 
        _json["obstalce"][num].push_back(maxx);
        _json["obstalce"][num].push_back(minz);
        _json["obstalce"][num].push_back(maxz);
    }


    /*-----スタート地点の設定------*/
    bool first_flag = true;
    float minx, minz, maxx, maxz;
    for (int y = 0; y < img.rows; ++y)
    {
        for (int x = 0; x < img.cols; ++x)
        {
            if (G[y][x] != 2) continue; 
            float l = float(x) * 150.0f - 7500.0f;
            float r = float(y) * 150.0f - 7500.0f;
            float l2 = float(x + 1) * 150.0f - 7500.0f;
            float r2 = float(y + 1) * 150.0f - 7500.0f;

            if (first_flag)
            {
                minx = l;
                maxx = l2;
                minz = r;
                maxz = r2;

                first_flag = false;
                continue;
            }

            if (minx > l) minx = l;
            if (maxx < l2) maxx = l2;
            if (minz > r) minz = r;
            if (maxz < r2) maxz = r2;
        }
    }

    if (first_flag)
    {
        printf("スタート地点が設定されていません\n"); 
        return false; 
    }

    float xLength = maxx - minx; 
    float zLength = maxz - minz; 

    _startPos.x = minx + xLength / 2.0f;
    _startPos.z = minz + zLength / 2.0f;

    if (xLength < zLength)
    {
        _startAngle = 3.14f / 2.0f; 
    }
    else
    {
        _startAngle = 0.0f; 
    }

    _json["startPosx"] = _startPos.x; 
    _json["startPosz"] = _startPos.z; 
    _json["startAngle"] = _startAngle; 

    /*-----道路の縁石の検出-----*/
    // 配列の初期化
    for (int y = 0; y < img.rows; ++y)
    {
        for (int x = 0; x < img.cols; ++x)
        {
            chk[y][x] = false; 
        }
    }

    for (int y = 0; y < img.rows; ++y)
    {
        for (int x = 0; x < img.cols; ++x)
        {
            if (G[y][x] != 1) continue;
            
            // (x+1, y) 
            if ((x + 1) < img.cols)
            {
                // ダートならば
                if (G[y][x + 1] == 0)
                {
                    G[y][x] = 5; 
                    _routeType[y * COL + x] = 5; 
                    _json["routeType"][y * COL + x] = 5;
                    continue; 
                }
            }
            
            // (x-1, y) 
            if ((x - 1) >= 0)
            {
                // ダートならば
                if (G[y][x - 1] == 0)
                {
                    G[y][x] = 5;
                    _routeType[y * COL + x] = 5;
                    _json["routeType"][y * COL + x] = 5;
                    continue;
                }
            }

            // (x, y+1) 
            if ((y + 1) < img.rows)
            {
                // ダートならば
                if (G[y+1][x] == 0)
                {
                    G[y][x] = 5;
                    _routeType[y * COL + x] = 5;
                    _json["routeType"][y * COL + x] = 5;
                    continue;
                }
            }

            // (x, y-1) 
            if ((y - 1) >= 0)
            {
                // ダートならば
                if (G[y - 1][x] == 0)
                {
                    G[y][x] = 5;
                    _routeType[y * COL + x] = 5;
                    _json["routeType"][y * COL + x] = 5;
                    continue;
                }
            }
        }
    }
    

    /*-----マテリアルの読み込み開始------*/
    printf("Rectangle-Floor::マテリアルの読み込み開始\n"); 

    // ダート
    descriptorHeap = new DescriptorHeap();
    auto texPath = ToWideString("Texture/grass.png");
    auto mainTex = Texture2D::Get(texPath);
    descriptorHandle = descriptorHeap->Register(mainTex);

    _descriptorHeaps.emplace_back(descriptorHeap); 
    _descriptorHandles.emplace_back(descriptorHandle); 

    // 道
    DescriptorHeap* desheap = new DescriptorHeap(); 
    texPath = ToWideString("Texture/stone.png");
    mainTex = Texture2D::Get(texPath);
    auto deshandle = desheap->Register(mainTex); 

    _descriptorHeaps.emplace_back(desheap);
    _descriptorHandles.emplace_back(deshandle);

    // スタート
    DescriptorHeap* desheap2 = new DescriptorHeap();
    texPath = ToWideString("Texture/red.png");
    mainTex = Texture2D::Get(texPath);
    auto deshandle2 = desheap2->Register(mainTex);

    _descriptorHeaps.emplace_back(desheap2);
    _descriptorHandles.emplace_back(deshandle2);

    // 障害物
    DescriptorHeap* desheap3 = new DescriptorHeap();
    texPath = ToWideString("Texture/stone.png");
    mainTex = Texture2D::Get(texPath);
    auto deshandle3 = desheap3->Register(mainTex);

    _descriptorHeaps.emplace_back(desheap3);
    _descriptorHandles.emplace_back(deshandle3);

    // 縁石
    DescriptorHeap* desheap4 = new DescriptorHeap();
    texPath = ToWideString("Texture/enseki.png");
    mainTex = Texture2D::Get(texPath);
    auto deshandle4 = desheap4->Register(mainTex);

    _descriptorHeaps.emplace_back(desheap4);
    _descriptorHandles.emplace_back(deshandle4);


    return true; 
}

// 現在座標が道の上か否かを返す関数
bool RouteMap::IsRoute(XMFLOAT3 pos)
{
    float x = pos.x + 7500.0f; 
    float z = pos.z + 7500.0f; 
    
    int col = static_cast<int>(x); 
    int row = static_cast<int>(z); 
    col /= 150; 
    row /= 150;

    int idx = row * 100 + col; 
    if (_routeType[idx] == 0)
    {
        return false; 
    }
    else
    {
        return true; 
    }
}

// 現在座標がアンカー上か否かを返す関数
bool RouteMap::IsAnchor(XMFLOAT3 pos)
{
    float x = pos.x + 7500.0f;
    float z = pos.z + 7500.0f;

    int col = static_cast<int>(x);
    int row = static_cast<int>(z);
    col /= 150;
    row /= 150;

    int idx = row * 100 + col;
    if (_routeType[idx] == 4)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// 現在座標がスタート地点か否かを返す関数
bool RouteMap::IsStart(XMFLOAT3 pos)
{
    float x = pos.x + 7500.0f;
    float z = pos.z + 7500.0f;

    int col = static_cast<int>(x);
    int row = static_cast<int>(z);
    col /= 150;
    row /= 150;

    int idx = row * 100 + col;
    if (_routeType[idx] == 2)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void RouteMap::Update()
{
    for (int i = 0; i < _obstacles.size(); ++i)
    {
        _obstacles[i]->Update(); 
    }
}

void RouteMap::Draw()
{
    auto commandList = g_Engine->CommandList();
    auto materialHeap = descriptorHeap->GetHeap(); 
    commandList->SetDescriptorHeaps(1, &materialHeap); // 使用するディスクリプタヒープのセット
    commandList->SetGraphicsRootDescriptorTable(2, descriptorHandle->HandleGPU); 

    /*------床の描画------*/ 
    for (int i = 0; i < _mapRects.size(); ++i)
    {
        auto commandList = g_Engine->CommandList();
        if (_routeType[i] == 0)
        {
            auto materialHeap = _descriptorHeaps[0]->GetHeap();
            commandList->SetDescriptorHeaps(1, &materialHeap); // 使用するディスクリプタヒープのセット
            commandList->SetGraphicsRootDescriptorTable(2, _descriptorHandles[0]->HandleGPU);
        }
        if (_routeType[i] == 1)
        {
            auto materialHeap = _descriptorHeaps[1]->GetHeap();
            commandList->SetDescriptorHeaps(1, &materialHeap); // 使用するディスクリプタヒープのセット
            commandList->SetGraphicsRootDescriptorTable(2, _descriptorHandles[1]->HandleGPU);
        }
        if (_routeType[i] == 2)
        {
            auto materialHeap = _descriptorHeaps[2]->GetHeap();
            commandList->SetDescriptorHeaps(1, &materialHeap); // 使用するディスクリプタヒープのセット
            commandList->SetGraphicsRootDescriptorTable(2, _descriptorHandles[2]->HandleGPU);
        }
        if (_routeType[i] == 3)
        {
            auto materialHeap = _descriptorHeaps[3]->GetHeap();
            commandList->SetDescriptorHeaps(1, &materialHeap); // 使用するディスクリプタヒープのセット
            commandList->SetGraphicsRootDescriptorTable(2, _descriptorHandles[3]->HandleGPU);
        }
        if (_routeType[i] == 4)
        {
            auto materialHeap = _descriptorHeaps[1]->GetHeap();
            commandList->SetDescriptorHeaps(1, &materialHeap); // 使用するディスクリプタヒープのセット
            commandList->SetGraphicsRootDescriptorTable(2, _descriptorHandles[1]->HandleGPU);
        }
        if (_routeType[i] == 5)
        {
            auto materialHeap = _descriptorHeaps[4]->GetHeap();
            commandList->SetDescriptorHeaps(1, &materialHeap); // 使用するディスクリプタヒープのセット
            commandList->SetGraphicsRootDescriptorTable(2, _descriptorHandles[4]->HandleGPU);
        }
        _mapRects[i]->Draw(); 
    }

    /*------障害物の描画------*/ 
    for (int i = 0; i < _obstacles.size(); ++i)
    {
        _obstacles[i]->Draw(); 
    }
}