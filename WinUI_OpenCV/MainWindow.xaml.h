#pragma once

#include "MainWindow.g.h"

//6-2�Œǉ��BOpenCV�̃w�b�_�[���C���N���[�h
#include <opencv2/opencv.hpp>

namespace winrt::WinUI_OpenCV::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow()
        {
            //6-1�Œǉ��B�^�C�g���ύX
			Title(L"WinUI_OpenCV");
        }

        //6-1�Œǉ��B�E�C���h�E�n���h���̎擾
        HWND GetHWND();

		//6-2�Œǉ��Bwstring����string�ւ̕ϊ�
		std::string WStringToString(const std::wstring& utf16);
		//6-2�Œǉ��Bstring����wstring�ւ̕ϊ�
		std::wstring StringToWString(const std::string& jisstr);

		//6-3�Œǉ��BMat����BitmapImage�ւ̕ϊ�
        winrt::Windows::Foundation::IAsyncOperation<winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage> MatToBitmapAsync(const cv::Mat& mat);

		//6-3�Œǉ��B�E�C���h�E�T�C�Y���摜�T�C�Y�ɍ��킹��
        void ResizeWindowToImage(const winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage& bitmap);

        //6-4�Œǉ��B�摜�T�C�Y��ύX
        void ResizeImage(const double scale);

        //�C�x���g�n���h��
        
		//6-1�ŕύX�B�񓯊���
        winrt::Windows::Foundation::IAsyncAction Open_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

        void Exit_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

        //6-4�ŕύX�B�񓯊���
        winrt::Windows::Foundation::IAsyncAction Enlarge2Times_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

        //6-4�ŕύX�B�񓯊���
        winrt::Windows::Foundation::IAsyncAction Reduce2Times_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

	private:
		//6-2�Œǉ��B�摜�f�[�^�ۑ������o
        cv::Mat _loadImg;

    };
}

namespace winrt::WinUI_OpenCV::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}

/**
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
**/
