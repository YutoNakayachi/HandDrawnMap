#include "App.h"
#include "Engine.h"
#include "Scene.h"
#include <SpriteFont.h>
#include <ResourceUploadBatch.h>
#include <time.h>
#include <sstream> 
#include<iostream>
#include<windows.h> 
#include"Helper.h"

#pragma comment(lib, "winmm.lib")


HINSTANCE g_hInst; // アプリケーション インスタンスハンドル
HWND g_hWnd = NULL; // アプリケーション ハンドルウィンドウ

HINSTANCE edit_hInst; // エディットコンソール インスタンスハンドル
HWND edit_hWnd = NULL; // エディットコンソール ハンドルウィンドウ

DWORD start; // ゲーム開始時刻
DWORD cur;  // 現在の時刻
DWORD goalTime; // ゴールした時刻

const float MIN_FRAME_TIME = (1.0f / 60.0f)*1000.0f; // FPSの設定

float recordTime; // 使用マップでのレコードタイム

bool edit_flag = true; // エディットコンソールの作成のために使うフラグ
bool mapInitFlag = true; // マップの初期化が終わっているか否かを判断するフラグ
bool startFlag = false;  // ゲーム開始後3秒間の待ち時間を作るためのフラグ
bool recordFlag = false; // レコード更新したか否かを判断するためのフラグ

DWORD offsetTime; // FPSを守るために使用する
float frameTime;  // FPSを守るために使用する

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        break;
    }

    return DefWindowProc(hWnd, msg, wp, lp);
}

// ウィンドウの初期化
void InitWindow(const TCHAR* appName, int width, int height, bool mode=true)
{
    // メインコンソールかエディットコンソールかをmode変数により判断する
    if (mode)
    {
        g_hInst = GetModuleHandle(nullptr);
        if (g_hInst == nullptr)
        {
            return;
        }
    }
    else
    {
        edit_hInst = GetModuleHandle(nullptr);
        if (edit_hInst == nullptr)
        {
            return;
        }
    }

    // ウィンドウの設定
    WNDCLASSEX wc = {};
    if (mode)
    {
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WndProc;
        wc.hIcon = LoadIcon(g_hInst, IDI_APPLICATION);
        wc.hCursor = LoadCursor(g_hInst, IDC_ARROW);
        wc.hbrBackground = GetSysColorBrush(COLOR_BACKGROUND);
        wc.lpszMenuName = nullptr;
        wc.lpszClassName = appName;
        wc.hIconSm = LoadIcon(g_hInst, IDI_APPLICATION);
    }
    else
    {
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WndProc;
        wc.hIcon = LoadIcon(edit_hInst, IDI_APPLICATION);
        wc.hCursor = LoadCursor(edit_hInst, IDC_ARROW);
        wc.hbrBackground = GetSysColorBrush(COLOR_BACKGROUND);
        wc.lpszMenuName = nullptr;
        wc.lpszClassName = appName;
        wc.hIconSm = LoadIcon(edit_hInst, IDI_APPLICATION);
    }

    // ウィンドウクラスの登録。
    RegisterClassEx(&wc);

    // ウィンドウサイズの設定
    RECT rect = {};
    rect.right = static_cast<LONG>(width);
    rect.bottom = static_cast<LONG>(height);

    // ウィンドウサイズを調整
    auto style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
    AdjustWindowRect(&rect, style, FALSE);

    // ウィンドウの生成
    if (mode)
    {
        g_hWnd = CreateWindowEx(
            0,
            appName,
            appName,
            style,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            rect.right - rect.left,
            rect.bottom - rect.top,
            nullptr,
            nullptr,
            g_hInst,
            nullptr
        );
    }
    else
    {
        edit_hWnd = CreateWindowEx(
            0,
            L"EDIT",
            appName,
            style,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            rect.right - rect.left,
            rect.bottom - rect.top,
            nullptr,
            nullptr,
            edit_hInst,
            nullptr
        );
    }

    // ウィンドウを表示
    if (mode)
    {
        ShowWindow(g_hWnd, SW_SHOWNORMAL);

        // ウィンドウにフォーカスする
        SetFocus(g_hWnd);
    }
    else
    {
        ShowWindow(edit_hWnd, SW_SHOWNORMAL);

        // ウィンドウにフォーカスする
        SetFocus(edit_hWnd);
    }
}

// タイトル画面の処理
void Title()
{
    // タイトル画面の更新
    g_Scene->TitleUpdate(); 

    // タイトル画面の描画前処理
    g_Engine->BeginRender(); 

    // タイトル画面の描画
    g_Scene->TitleDraw(); 

    // タイトル画面の文字描画
    g_Engine->CommandList()->SetDescriptorHeaps(1, g_Engine->_heapForSpriteFont.GetAddressOf());
    g_Engine->_spriteBatch->Begin(g_Engine->CommandList());

    const wchar_t* titleChr = L"Press Enter key"; 

    g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, titleChr, DirectX::XMFLOAT2(502.0f, 502.0f), DirectX::Colors::White);
    g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, titleChr, DirectX::XMFLOAT2(500.0f, 500.0f), DirectX::Colors::Black);

    g_Engine->_spriteBatch->End();
    g_Engine->_gmemory->Commit(g_Engine->CommandQueue());

    // タイトル画面の描画後処理
    g_Engine->EndRender(); 

    // FPS固定の処理
    cur = timeGetTime();
    auto dif = cur - offsetTime;
    frameTime = static_cast<float>(dif);
    //std::cout << "frameTime: " << frameTime << std::endl;
    if( (MIN_FRAME_TIME - frameTime)>=0.0f)
    {
        Sleep((MIN_FRAME_TIME - frameTime));
    }
    offsetTime = timeGetTime(); 
}

// マップ選択画面の処理
void ChoiceMap()
{
    // マップモードが選択されるまでの処理
    if (g_Scene->_mode == -1)
    {
        g_Scene->MapChoiceUpdate(); 

        g_Engine->BeginRender();

        g_Scene->MapChoiceDraw();

        g_Engine->EndRender(); 

        return; 
    }
    
    // エディットコンソール作成の処理
    if (edit_flag)
    {
        // エディットコンソールの作成
        InitWindow(L"MAPのパスを入力してください", 1000, 200, false);
        
        // MAPパスの初期設定
        if (g_Scene->_mode == 1)
        {
            SetWindowText(edit_hWnd, L"mapimg\\\\defaultRoute1.png");
        }
        else if (g_Scene->_mode == 2)
        {
            SetWindowText(edit_hWnd, L"mapbin\\\\defaultRoute1.bin");
        }
        edit_flag = false;
    }

    wchar_t wc[1024];
    if (GetAsyncKeyState(VK_RETURN) & 0x0001)
    {
        int getnum = GetWindowText(edit_hWnd, wc, GetWindowTextLength(edit_hWnd) + 1);

        std::wstringstream ss;
        ss << wc;
        std::wstring wstr = ss.str();

        g_Scene->_mapName = ToUTF8(wstr);

        std::cout << "mapName:" << g_Scene->_mapName << std::endl;

        // エディットウィンドウの削除 + 次のシーンへ
        if (DestroyWindow(edit_hWnd))
        {
            printf("エディットウィンドウの消去に成功\n");
            g_Scene->_state++;
        }
        else
        {
            printf("エディットウィンドウの消去に失敗\n");
        }
    }
}

// マップロード画面の処理
void MapLoad()
{
    // マップの初期化
    if (mapInitFlag)
    {
        // 文字描画
        const wchar_t* choiceMapChr = L"Map Loading...";
        g_Engine->BeginRender();

        g_Scene->MapLoadDraw();

        g_Engine->CommandList()->SetDescriptorHeaps(1, g_Engine->_heapForSpriteFont.GetAddressOf());
        g_Engine->_spriteBatch->Begin(g_Engine->CommandList());

        g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, choiceMapChr, DirectX::XMFLOAT2(502.0f, 602.0f), DirectX::Colors::Black);
        g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, choiceMapChr, DirectX::XMFLOAT2(500.0f, 600.0f), DirectX::Colors::Red);

        g_Engine->_spriteBatch->End();
        g_Engine->_gmemory->Commit(g_Engine->CommandQueue());

        g_Engine->EndRender();
        if (!g_Scene->InitMap())
        {
            printf("マップの初期化に失敗しました\n");

        }
        mapInitFlag = false;
    }
    else
    {
        // 文字描画
        const wchar_t* choiceMapChr = L"Start Press Enter!";
        g_Engine->BeginRender();

        g_Scene->MapLoadDraw();

        g_Engine->CommandList()->SetDescriptorHeaps(1, g_Engine->_heapForSpriteFont.GetAddressOf());
        g_Engine->_spriteBatch->Begin(g_Engine->CommandList());

        g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, choiceMapChr, DirectX::XMFLOAT2(502.0f, 602.0f), DirectX::Colors::Black);
        g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, choiceMapChr, DirectX::XMFLOAT2(500.0f, 600.0f), DirectX::Colors::Red);

        g_Engine->_spriteBatch->End();
        g_Engine->_gmemory->Commit(g_Engine->CommandQueue());

        g_Engine->EndRender();

        // Enterキーが押されると次のシーンへ
        if (GetAsyncKeyState(VK_RETURN) & 0x0001)
        {
            g_Scene->_state++;
            start = timeGetTime();

            g_Scene->InitLast();
        }

        // FPS固定の処理
        cur = timeGetTime();
        auto dif = cur - offsetTime;
        frameTime = static_cast<float>(dif);
        //std::cout << "frameTime: " << frameTime << std::endl;
        if ((MIN_FRAME_TIME - frameTime) >= 0.0f)
        {
            Sleep((MIN_FRAME_TIME - frameTime));
        }
        offsetTime = timeGetTime();
    }
}

// メインとなるゲームシーンの処理
void Game()
{
    
    if (startFlag)
    {
        g_Scene->Update();
    }

    // 描画前処理
    g_Engine->BeginRender();

    // 描画
    g_Scene->Draw();

    // 文字描画処理
    g_Engine->CommandList()->SetDescriptorHeaps(1, g_Engine->_heapForSpriteFont.GetAddressOf());
    g_Engine->_spriteBatch->Begin(g_Engine->CommandList());

    // ゴールしていない時の処理
    if (g_Scene->_cycle <= 3)
    {
        goalTime = timeGetTime();
        std::wstringstream cy_ss;

        const wchar_t* cy_ext = L"/ 3";
        cy_ss << g_Scene->_cycle;
        std::wstring cy_str = cy_ss.str();
        const wchar_t* cy_wc = cy_str.c_str();

        // 時間描画
        cur = timeGetTime();
        std::wstringstream ss;
        float timeVal; 

        if (startFlag)
        {
            auto dif = cur - start;
            timeVal = static_cast<float>(dif / 1000.0);
            timeVal -= 3.0f; 
        }
        else
        {
            auto dif = cur - start;
            timeVal = static_cast<float>(dif / 1000.0);
            timeVal = 3.0f - timeVal; 
        }

        if (timeVal <= 0.0f)
        {
            startFlag = true; 
        }

        ss << timeVal;
        std::wstring str = ss.str();
        const wchar_t* wc = str.c_str();

        // 待ち時間を過ぎた時の処理
        if (startFlag)
        {
            if (timeVal <= 1.0f)
            {
                const wchar_t* startWc = L"START";
                g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, startWc, DirectX::XMFLOAT2(552.0f, 132.0f), DirectX::Colors::Black);
                g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, startWc, DirectX::XMFLOAT2(550.0f, 130.0f), DirectX::Colors::Red);
            }

            // 時間の描画
            const wchar_t* timeChr = L"TIME :";
            g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, timeChr, DirectX::XMFLOAT2(2.0f, 132.0f), DirectX::Colors::Black);
            g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, timeChr, DirectX::XMFLOAT2(0.0f, 130.0f), DirectX::Colors::Gray);
            g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, wc, DirectX::XMFLOAT2(202.0f, 132.0f), DirectX::Colors::Black);
            g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, wc, DirectX::XMFLOAT2(200.0f, 130.0f), DirectX::Colors::SeaGreen);
        }
        // 待ち時間中の処理
        else
        {
            if (timeVal >= 2.0f)
            {
                str = L"3"; 
            }
            else if (timeVal >= 1.0f)
            {
                str = L"2";
            }
            else if (timeVal >= 0.0f)
            {
                str = L"1";
            }
            const wchar_t* waitWc = str.c_str(); 
            g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, waitWc, DirectX::XMFLOAT2(642.0f, 132.0f), DirectX::Colors::Black);
            g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, waitWc, DirectX::XMFLOAT2(640.0f, 130.0f), DirectX::Colors::Red);
        }

        // 暫定のレコードタイムの描画
        if (g_Scene->_routemap->_recordTime > 0.0f)
        {
            std::wstringstream record_ss;
            record_ss << g_Scene->_routemap->_recordTime;
            std::wstring record_str = record_ss.str();
            const wchar_t* record_wc = record_str.c_str();

            const wchar_t* timeChr = L"RECORD:";
            g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, timeChr, DirectX::XMFLOAT2(2.0f, 232.0f), DirectX::Colors::Black);
            g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, timeChr, DirectX::XMFLOAT2(0.0f, 230.0f), DirectX::Colors::Gray);
            g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, record_wc, DirectX::XMFLOAT2(232.0f, 232.0f), DirectX::Colors::Black);
            g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, record_wc, DirectX::XMFLOAT2(230.0f, 230.0f), DirectX::Colors::Yellow);
        }

        // コース周回回数の描画
        g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, cy_wc, DirectX::XMFLOAT2(1002.0f, 132.0f), DirectX::Colors::White);
        g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, cy_wc, DirectX::XMFLOAT2(1000.0f, 130.0f), DirectX::Colors::Black);

        g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, cy_ext, DirectX::XMFLOAT2(1052.0f, 132.0f), DirectX::Colors::White);
        g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, cy_ext, DirectX::XMFLOAT2(1050.0f, 130.0f), DirectX::Colors::Black);
    }
    // ゴールしたときの処理
    else
    {
        const wchar_t* goalChr = L"GOAL!!";

        // ゴールタイムを文字列に変換
        auto dif = goalTime - start;
        float timeVal = static_cast<float>(dif / 1000.0) - 3.0f;
        std::wstringstream ss;
        ss << timeVal;
        std::wstring str = ss.str();
        const wchar_t* wc = str.c_str();

        // ゴール時間描画
        g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, wc, DirectX::XMFLOAT2(2.0f, 132.0f), DirectX::Colors::Black);
        g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, wc, DirectX::XMFLOAT2(0.0f, 130.0f), DirectX::Colors::SeaGreen);

        // ゴール文字描画
        g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, goalChr, DirectX::XMFLOAT2(752.0f, 500.0f), DirectX::Colors::Black);
        g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, goalChr, DirectX::XMFLOAT2(750.0f, 500.0f), DirectX::Colors::Red);

        g_Scene->_state++; 
    }
    g_Engine->_spriteBatch->End();
    g_Engine->_gmemory->Commit(g_Engine->CommandQueue());

    // 描画後処理
    g_Engine->EndRender();


    // FPS固定の処理
    cur = timeGetTime();
    auto dif = cur - offsetTime;
    frameTime = static_cast<float>(dif);
    if ((MIN_FRAME_TIME - frameTime) >= 0.0f)
    {
        Sleep((MIN_FRAME_TIME - frameTime));
    }
    offsetTime = timeGetTime();
}

// リザルトシーンの処理
void Result()
{
    // リザルトシーンの更新処理
    g_Scene->ResultUpdate();

    // 描画前処理
    g_Engine->BeginRender();

    // リザルトシーンの描画
    g_Scene->Draw(); 

    // 描画後処理
    g_Scene->ResultDraw();

    g_Engine->CommandList()->SetDescriptorHeaps(1, g_Engine->_heapForSpriteFont.GetAddressOf());
    g_Engine->_spriteBatch->Begin(g_Engine->CommandList());

    const wchar_t* resultChr = L"GOAL!!";

    // ゴールタイムの文字列への変換
    auto dif = goalTime - start;
    float timeVal = static_cast<float>(dif / 1000.0) - 3.0f;
    std::wstringstream ss;
    ss << timeVal;
    std::wstring str = ss.str();
    const wchar_t* wc = str.c_str();

    recordTime = timeVal; 

    // ゴール時間描画
    const wchar_t* timeChr = L"TIME :";
    g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, timeChr, DirectX::XMFLOAT2(2.0f, 132.0f), DirectX::Colors::Black);
    g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, timeChr, DirectX::XMFLOAT2(0.0f, 130.0f), DirectX::Colors::Gray);

    g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, wc, DirectX::XMFLOAT2(202.0f, 132.0f), DirectX::Colors::Black);
    g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, wc, DirectX::XMFLOAT2(200.0f, 130.0f), DirectX::Colors::SeaGreen);

    // ニューレコードか否かの判定
    if (g_Scene->_routemap->_recordTime > 0.0f && g_Scene->_routemap->_recordTime > recordTime)
    {
        recordFlag = true; 
    }
    else if (g_Scene->_routemap->_recordTime < 0.0f)
    {
        recordFlag = true; 
    }

    // ニューレコードのときの処理
    if (recordFlag)
    {
        const wchar_t* newRecordChr = L"NEW RECORD!!!";
        g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, newRecordChr, DirectX::XMFLOAT2(2.0f, 232.0f), DirectX::Colors::Black);
        g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, newRecordChr, DirectX::XMFLOAT2(0.0f, 230.0f), DirectX::Colors::Red);
    }

    // ゴール文字描画
    g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, resultChr, DirectX::XMFLOAT2(542.0f, 302.0f), DirectX::Colors::Black);
    g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, resultChr, DirectX::XMFLOAT2(540.0f, 300.0f), DirectX::Colors::Red);

    // ゲーム終了描画
    const wchar_t* endChr = L"END Press Enter";
    g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, endChr, DirectX::XMFLOAT2(502.0f, 402.0f), DirectX::Colors::Black);
    g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, endChr, DirectX::XMFLOAT2(500.0f, 400.0f), DirectX::Colors::Gray);

    g_Engine->_spriteBatch->End();
    g_Engine->_gmemory->Commit(g_Engine->CommandQueue());

    // 描画後処理
    g_Engine->EndRender();

    // FPS固定の処理
    cur = timeGetTime();
    dif = cur - offsetTime;
    frameTime = static_cast<float>(dif);
    if ((MIN_FRAME_TIME - frameTime) >= 0.0f)
    {
        Sleep((MIN_FRAME_TIME - frameTime));
    }
    offsetTime = timeGetTime();
}

// アプリケーション メインループ
void MainLoop()
{
    MSG msg = {};
    offsetTime = timeGetTime(); 
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE == TRUE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            if (g_Scene->_state == 0)
            {
                Title(); 
            }
            else if (g_Scene->_state == 1)
            {
                ChoiceMap();
            }
            else if (g_Scene->_state == 2)
            {
                MapLoad(); 
            }
            else if(g_Scene->_state==3)
            {
                Game();
            }
            else if(g_Scene->_state==4)
            {
                Result(); 
            }
            else
            {
                // レコードタイムの更新
                if (recordFlag)
                {
                    g_Scene->_routemap->_recordTime = recordTime;
                }

                // バイナリファイルへの書き込み
                g_Scene->_routemap->writeJsonFile(g_Scene->_mapName);

                // ウィンドウ消去
                DestroyWindow(g_hWnd); 
            }
        }
    }
}

// アプリケーションを開始する関数
void StartApp(const TCHAR* appName)
{
    // ウィンドウ生成
    InitWindow(appName, WINDOW_WIDTH, WINDOW_HEIGHT);

    // 描画エンジンの初期化を行う
    g_Engine = new Engine();
    if (!g_Engine->Init(g_hWnd, WINDOW_WIDTH, WINDOW_HEIGHT))
    {
        printf("描画エンジンの初期化に失敗しました"); 
        return;
    }

    // シーン初期化
    g_Scene = new Scene();
    if (!g_Scene->Init())
    {
        printf("シーンの初期化に失敗しました");
        return;
    }

    // メイン処理ループ
    MainLoop();
}
