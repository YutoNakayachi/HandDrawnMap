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


HINSTANCE g_hInst; // �A�v���P�[�V���� �C���X�^���X�n���h��
HWND g_hWnd = NULL; // �A�v���P�[�V���� �n���h���E�B���h�E

HINSTANCE edit_hInst; // �G�f�B�b�g�R���\�[�� �C���X�^���X�n���h��
HWND edit_hWnd = NULL; // �G�f�B�b�g�R���\�[�� �n���h���E�B���h�E

DWORD start; // �Q�[���J�n����
DWORD cur;  // ���݂̎���
DWORD goalTime; // �S�[����������

const float MIN_FRAME_TIME = (1.0f / 60.0f)*1000.0f; // FPS�̐ݒ�

float recordTime; // �g�p�}�b�v�ł̃��R�[�h�^�C��

bool edit_flag = true; // �G�f�B�b�g�R���\�[���̍쐬�̂��߂Ɏg���t���O
bool mapInitFlag = true; // �}�b�v�̏��������I����Ă��邩�ۂ��𔻒f����t���O
bool startFlag = false;  // �Q�[���J�n��3�b�Ԃ̑҂����Ԃ���邽�߂̃t���O
bool recordFlag = false; // ���R�[�h�X�V�������ۂ��𔻒f���邽�߂̃t���O

DWORD offsetTime; // FPS����邽�߂Ɏg�p����
float frameTime;  // FPS����邽�߂Ɏg�p����

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

// �E�B���h�E�̏�����
void InitWindow(const TCHAR* appName, int width, int height, bool mode=true)
{
    // ���C���R���\�[�����G�f�B�b�g�R���\�[������mode�ϐ��ɂ�蔻�f����
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

    // �E�B���h�E�̐ݒ�
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

    // �E�B���h�E�N���X�̓o�^�B
    RegisterClassEx(&wc);

    // �E�B���h�E�T�C�Y�̐ݒ�
    RECT rect = {};
    rect.right = static_cast<LONG>(width);
    rect.bottom = static_cast<LONG>(height);

    // �E�B���h�E�T�C�Y�𒲐�
    auto style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
    AdjustWindowRect(&rect, style, FALSE);

    // �E�B���h�E�̐���
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

    // �E�B���h�E��\��
    if (mode)
    {
        ShowWindow(g_hWnd, SW_SHOWNORMAL);

        // �E�B���h�E�Ƀt�H�[�J�X����
        SetFocus(g_hWnd);
    }
    else
    {
        ShowWindow(edit_hWnd, SW_SHOWNORMAL);

        // �E�B���h�E�Ƀt�H�[�J�X����
        SetFocus(edit_hWnd);
    }
}

// �^�C�g����ʂ̏���
void Title()
{
    // �^�C�g����ʂ̍X�V
    g_Scene->TitleUpdate(); 

    // �^�C�g����ʂ̕`��O����
    g_Engine->BeginRender(); 

    // �^�C�g����ʂ̕`��
    g_Scene->TitleDraw(); 

    // �^�C�g����ʂ̕����`��
    g_Engine->CommandList()->SetDescriptorHeaps(1, g_Engine->_heapForSpriteFont.GetAddressOf());
    g_Engine->_spriteBatch->Begin(g_Engine->CommandList());

    const wchar_t* titleChr = L"Press Enter key"; 

    g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, titleChr, DirectX::XMFLOAT2(502.0f, 502.0f), DirectX::Colors::White);
    g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, titleChr, DirectX::XMFLOAT2(500.0f, 500.0f), DirectX::Colors::Black);

    g_Engine->_spriteBatch->End();
    g_Engine->_gmemory->Commit(g_Engine->CommandQueue());

    // �^�C�g����ʂ̕`��㏈��
    g_Engine->EndRender(); 

    // FPS�Œ�̏���
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

// �}�b�v�I����ʂ̏���
void ChoiceMap()
{
    // �}�b�v���[�h���I�������܂ł̏���
    if (g_Scene->_mode == -1)
    {
        g_Scene->MapChoiceUpdate(); 

        g_Engine->BeginRender();

        g_Scene->MapChoiceDraw();

        g_Engine->EndRender(); 

        return; 
    }
    
    // �G�f�B�b�g�R���\�[���쐬�̏���
    if (edit_flag)
    {
        // �G�f�B�b�g�R���\�[���̍쐬
        InitWindow(L"MAP�̃p�X����͂��Ă�������", 1000, 200, false);
        
        // MAP�p�X�̏����ݒ�
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

        // �G�f�B�b�g�E�B���h�E�̍폜 + ���̃V�[����
        if (DestroyWindow(edit_hWnd))
        {
            printf("�G�f�B�b�g�E�B���h�E�̏����ɐ���\n");
            g_Scene->_state++;
        }
        else
        {
            printf("�G�f�B�b�g�E�B���h�E�̏����Ɏ��s\n");
        }
    }
}

// �}�b�v���[�h��ʂ̏���
void MapLoad()
{
    // �}�b�v�̏�����
    if (mapInitFlag)
    {
        // �����`��
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
            printf("�}�b�v�̏������Ɏ��s���܂���\n");

        }
        mapInitFlag = false;
    }
    else
    {
        // �����`��
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

        // Enter�L�[���������Ǝ��̃V�[����
        if (GetAsyncKeyState(VK_RETURN) & 0x0001)
        {
            g_Scene->_state++;
            start = timeGetTime();

            g_Scene->InitLast();
        }

        // FPS�Œ�̏���
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

// ���C���ƂȂ�Q�[���V�[���̏���
void Game()
{
    
    if (startFlag)
    {
        g_Scene->Update();
    }

    // �`��O����
    g_Engine->BeginRender();

    // �`��
    g_Scene->Draw();

    // �����`�揈��
    g_Engine->CommandList()->SetDescriptorHeaps(1, g_Engine->_heapForSpriteFont.GetAddressOf());
    g_Engine->_spriteBatch->Begin(g_Engine->CommandList());

    // �S�[�����Ă��Ȃ����̏���
    if (g_Scene->_cycle <= 3)
    {
        goalTime = timeGetTime();
        std::wstringstream cy_ss;

        const wchar_t* cy_ext = L"/ 3";
        cy_ss << g_Scene->_cycle;
        std::wstring cy_str = cy_ss.str();
        const wchar_t* cy_wc = cy_str.c_str();

        // ���ԕ`��
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

        // �҂����Ԃ��߂������̏���
        if (startFlag)
        {
            if (timeVal <= 1.0f)
            {
                const wchar_t* startWc = L"START";
                g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, startWc, DirectX::XMFLOAT2(552.0f, 132.0f), DirectX::Colors::Black);
                g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, startWc, DirectX::XMFLOAT2(550.0f, 130.0f), DirectX::Colors::Red);
            }

            // ���Ԃ̕`��
            const wchar_t* timeChr = L"TIME :";
            g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, timeChr, DirectX::XMFLOAT2(2.0f, 132.0f), DirectX::Colors::Black);
            g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, timeChr, DirectX::XMFLOAT2(0.0f, 130.0f), DirectX::Colors::Gray);
            g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, wc, DirectX::XMFLOAT2(202.0f, 132.0f), DirectX::Colors::Black);
            g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, wc, DirectX::XMFLOAT2(200.0f, 130.0f), DirectX::Colors::SeaGreen);
        }
        // �҂����Ԓ��̏���
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

        // �b��̃��R�[�h�^�C���̕`��
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

        // �R�[�X����񐔂̕`��
        g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, cy_wc, DirectX::XMFLOAT2(1002.0f, 132.0f), DirectX::Colors::White);
        g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, cy_wc, DirectX::XMFLOAT2(1000.0f, 130.0f), DirectX::Colors::Black);

        g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, cy_ext, DirectX::XMFLOAT2(1052.0f, 132.0f), DirectX::Colors::White);
        g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, cy_ext, DirectX::XMFLOAT2(1050.0f, 130.0f), DirectX::Colors::Black);
    }
    // �S�[�������Ƃ��̏���
    else
    {
        const wchar_t* goalChr = L"GOAL!!";

        // �S�[���^�C���𕶎���ɕϊ�
        auto dif = goalTime - start;
        float timeVal = static_cast<float>(dif / 1000.0) - 3.0f;
        std::wstringstream ss;
        ss << timeVal;
        std::wstring str = ss.str();
        const wchar_t* wc = str.c_str();

        // �S�[�����ԕ`��
        g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, wc, DirectX::XMFLOAT2(2.0f, 132.0f), DirectX::Colors::Black);
        g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, wc, DirectX::XMFLOAT2(0.0f, 130.0f), DirectX::Colors::SeaGreen);

        // �S�[�������`��
        g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, goalChr, DirectX::XMFLOAT2(752.0f, 500.0f), DirectX::Colors::Black);
        g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, goalChr, DirectX::XMFLOAT2(750.0f, 500.0f), DirectX::Colors::Red);

        g_Scene->_state++; 
    }
    g_Engine->_spriteBatch->End();
    g_Engine->_gmemory->Commit(g_Engine->CommandQueue());

    // �`��㏈��
    g_Engine->EndRender();


    // FPS�Œ�̏���
    cur = timeGetTime();
    auto dif = cur - offsetTime;
    frameTime = static_cast<float>(dif);
    if ((MIN_FRAME_TIME - frameTime) >= 0.0f)
    {
        Sleep((MIN_FRAME_TIME - frameTime));
    }
    offsetTime = timeGetTime();
}

// ���U���g�V�[���̏���
void Result()
{
    // ���U���g�V�[���̍X�V����
    g_Scene->ResultUpdate();

    // �`��O����
    g_Engine->BeginRender();

    // ���U���g�V�[���̕`��
    g_Scene->Draw(); 

    // �`��㏈��
    g_Scene->ResultDraw();

    g_Engine->CommandList()->SetDescriptorHeaps(1, g_Engine->_heapForSpriteFont.GetAddressOf());
    g_Engine->_spriteBatch->Begin(g_Engine->CommandList());

    const wchar_t* resultChr = L"GOAL!!";

    // �S�[���^�C���̕�����ւ̕ϊ�
    auto dif = goalTime - start;
    float timeVal = static_cast<float>(dif / 1000.0) - 3.0f;
    std::wstringstream ss;
    ss << timeVal;
    std::wstring str = ss.str();
    const wchar_t* wc = str.c_str();

    recordTime = timeVal; 

    // �S�[�����ԕ`��
    const wchar_t* timeChr = L"TIME :";
    g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, timeChr, DirectX::XMFLOAT2(2.0f, 132.0f), DirectX::Colors::Black);
    g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, timeChr, DirectX::XMFLOAT2(0.0f, 130.0f), DirectX::Colors::Gray);

    g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, wc, DirectX::XMFLOAT2(202.0f, 132.0f), DirectX::Colors::Black);
    g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, wc, DirectX::XMFLOAT2(200.0f, 130.0f), DirectX::Colors::SeaGreen);

    // �j���[���R�[�h���ۂ��̔���
    if (g_Scene->_routemap->_recordTime > 0.0f && g_Scene->_routemap->_recordTime > recordTime)
    {
        recordFlag = true; 
    }
    else if (g_Scene->_routemap->_recordTime < 0.0f)
    {
        recordFlag = true; 
    }

    // �j���[���R�[�h�̂Ƃ��̏���
    if (recordFlag)
    {
        const wchar_t* newRecordChr = L"NEW RECORD!!!";
        g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, newRecordChr, DirectX::XMFLOAT2(2.0f, 232.0f), DirectX::Colors::Black);
        g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, newRecordChr, DirectX::XMFLOAT2(0.0f, 230.0f), DirectX::Colors::Red);
    }

    // �S�[�������`��
    g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, resultChr, DirectX::XMFLOAT2(542.0f, 302.0f), DirectX::Colors::Black);
    g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, resultChr, DirectX::XMFLOAT2(540.0f, 300.0f), DirectX::Colors::Red);

    // �Q�[���I���`��
    const wchar_t* endChr = L"END Press Enter";
    g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, endChr, DirectX::XMFLOAT2(502.0f, 402.0f), DirectX::Colors::Black);
    g_Engine->_spriteFont->DrawString(g_Engine->_spriteBatch, endChr, DirectX::XMFLOAT2(500.0f, 400.0f), DirectX::Colors::Gray);

    g_Engine->_spriteBatch->End();
    g_Engine->_gmemory->Commit(g_Engine->CommandQueue());

    // �`��㏈��
    g_Engine->EndRender();

    // FPS�Œ�̏���
    cur = timeGetTime();
    dif = cur - offsetTime;
    frameTime = static_cast<float>(dif);
    if ((MIN_FRAME_TIME - frameTime) >= 0.0f)
    {
        Sleep((MIN_FRAME_TIME - frameTime));
    }
    offsetTime = timeGetTime();
}

// �A�v���P�[�V���� ���C�����[�v
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
                // ���R�[�h�^�C���̍X�V
                if (recordFlag)
                {
                    g_Scene->_routemap->_recordTime = recordTime;
                }

                // �o�C�i���t�@�C���ւ̏�������
                g_Scene->_routemap->writeJsonFile(g_Scene->_mapName);

                // �E�B���h�E����
                DestroyWindow(g_hWnd); 
            }
        }
    }
}

// �A�v���P�[�V�������J�n����֐�
void StartApp(const TCHAR* appName)
{
    // �E�B���h�E����
    InitWindow(appName, WINDOW_WIDTH, WINDOW_HEIGHT);

    // �`��G���W���̏��������s��
    g_Engine = new Engine();
    if (!g_Engine->Init(g_hWnd, WINDOW_WIDTH, WINDOW_HEIGHT))
    {
        printf("�`��G���W���̏������Ɏ��s���܂���"); 
        return;
    }

    // �V�[��������
    g_Scene = new Scene();
    if (!g_Scene->Init())
    {
        printf("�V�[���̏������Ɏ��s���܂���");
        return;
    }

    // ���C���������[�v
    MainLoop();
}
