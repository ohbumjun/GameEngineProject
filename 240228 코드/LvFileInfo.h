#pragma once
#ifndef __LV_FILE_INFO_H__
#define __LV_FILE_INFO_H__

#include "system/LvString.h"

/* #include "editor/private/project/LvFileTable.h" */ namespace Lv { namespace Editor { namespace Project { class LvFileTable; } } }

LV_NS_EDITOR_BEGIN

namespace Project
{
	/**
	 * @brief 복사하여 관리될 수 있는 파일이나 디렉토리 정보
	 * @file #include "editor/private/project/LvFileInfo.h"
	 */
	struct LvFileInfo
	{
		/**
		 * @brief 파일 이름
		 */
		LvString name;

		/**
		 * @brief 프로젝트 폴더를 기준으로 한 상대경로
		 */
		LvString path;

		/**
		 * @brief 절대 경로
		 */
		LvString absolutePath;

		/**
		 * @brief 파일 확장자
		 */
		LvString extension;

		/**
		 * @brief 디렉토리 여부
		 */
		bool isDirectory;

		/**
		 * @brief .inf 파일 여부
		 */
		bool isInf;

		/**
		 * @brief 데이터 유효 여부, false인 경우 잘못된 LvFileInfo를 참조중인 것으로 간주
		 */
		bool isValid;

		LvFileInfo();
		LvFileInfo(const char * absolutePath, const LvFileTable* baseTable);
		~LvFileInfo() { isValid = false; }
		LvFileInfo(const LvFileInfo&) = default;
		LvFileInfo& operator =(const LvFileInfo&);
		LvFileInfo(LvFileInfo&&) = default;
		LvFileInfo& operator = (LvFileInfo&&) = default;

		
		/**
		 * @return 이름으로 하위 파일 혹은 디렉토리 정보를 검색
		 */
		const LvFileInfo& FindChild(const char* childName) const;

		/**
		 * @return 하위 파일 혹은 디렉토리의 절대경로들을 반환
		 */
		const LvList<LvString>& GetChildAbsolutePaths() const;

		/**
		 * @brief 프로젝트 폴더 내 Resources 폴더를 기준으로 한 상대경로
		 */
		LvString GetResourcesRelativePath() const;

		/**
		 * @brief 절대경로를 통해 프로젝트 폴더 내 Resources 폴더를 기준의 상대경로 변환
		 * @param absolutePath 대상의 절대경로 ( "C:/projects/test/Resources/target.png" )
		 * @return Resources 폴더를 기준의 상대경로 ( return "target.png"; )
		 * @code
		 * LvFileInfo::ToResourcesRelativePathAtAbsolutePath("C:/projects/test/Resources/target.png")
		 * @endcode
		 */
		static LvString ToResourcesRelativePathAtAbsolutePath(const LvString& absolutePath) { return ToResourcesRelativePathAtAbsolutePath(absolutePath.c_str()); }

		/**
		 * @brief 절대경로를 통해 프로젝트 폴더 내 Resources 폴더를 기준의 상대경로 변환
		 * @param absolutePath 대상의 절대경로 ( "C:/projects/test/Resources/target.png" )
		 * @return Resources 폴더를 기준의 상대경로 ( return "target.png"; )
		 * @code
		 * LvFileInfo::ToResourcesRelativePathAtAbsolutePath("C:/projects/test/Resources/target.png")
		 * @endcode
		 */
		static LvString ToResourcesRelativePathAtAbsolutePath(const char* absolutePath);

		/**
		 * @brief 프로젝트 폴더의 상대경로를 통해 Resources 폴더 기준의 상대경로 변환
		 * @param relativePath 대상의 프로젝트 폴더 기준 상대경로 ( "Resources/target.png" )
		 * @return Resources 폴더를 기준의 상대경로 ( return "target.png"; )
		 * @code
		 * LvFileInfo::ToResourcesRelativePathAtRelativePath("Resources/target.png")
		 * @endcode
		 */
		static LvString ToResourcesRelativePathAtRelativePath(const LvString& relativePath) { return ToResourcesRelativePathAtRelativePath(relativePath.c_str()); }

		/**
		 * @brief 프로젝트 폴더의 상대경로를 통해 Resources 폴더 기준의 상대경로 변환
		 * @param relativePath 대상의 프로젝트 폴더 기준 상대경로 ( "Resources/target.png" )
		 * @return Resources 폴더를 기준의 상대경로 ( return "target.png"; )
		 * @code
		 * LvFileInfo::ToResourcesRelativePathAtRelativePath("Resources/target.png")
		 * @endcode
		 */
		static LvString ToResourcesRelativePathAtRelativePath(const char* relativePath);

		/**
		 * @brief Resources 폴더 기준의 상대경로를 통해 절대경로 변환
		 * @param relativePath Resources 폴더를 기준의 상대경로 ( "target.png" )
		 * @return 대상의 절대경로 ( return "C:/projects/test/Resources/target.png"; )
		 * @code
		 * LvFileInfo::ToAbsolutePathAtResourcesRelativePath("target.png")
		 * @endcode
		 */
		static LvString ToAbsolutePathAtResourcesRelativePath(const LvString& relativePath) { return ToAbsolutePathAtResourcesRelativePath(relativePath.c_str()); }
		/**
		 * @brief Resources 폴더 기준의 상대경로를 통해 절대경로 변환
		 * @param relativePath Resources 폴더를 기준의 상대경로 ( "target.png" )
		 * @return 대상의 절대경로 ( return "C:/projects/test/Resources/target.png"; )
		 * @code
		 * LvFileInfo::ToAbsolutePathAtResourcesRelativePath("target.png")
		 * @endcode
		 */
		static LvString ToAbsolutePathAtResourcesRelativePath(const char* relativePath);

		/**
		 * @brief Resources 폴더 기준의 상대경로를 통해 프로젝트 폴더의 상대경로 변환
		 * @param relativePath Resources 폴더를 기준의 상대경로 ( "target.png" )
		 * @return 대상의 프로젝트 폴더 기준 상대경로 ( return "Resources/target.png"; )
		 * @code LvFileInfo::ToRelativePathAtResourcesRelativePath("target.png")
		 * @endcode
		 */
		static LvString ToRelativePathAtResourcesRelativePath(const LvString& relativePath) { return ToRelativePathAtResourcesRelativePath(relativePath.c_str()); }

		/**
		 * @brief Resources 폴더 기준의 상대경로를 통해 프로젝트 폴더의 상대경로 변환
		 * @param relativePath Resources 폴더를 기준의 상대경로 ( "target.png" )
		 * @return 대상의 프로젝트 폴더 기준 상대경로 ( return "Resources/target.png"; )
		 * @code LvFileInfo::ToRelativePathAtResourcesRelativePath("target.png")
		 * @endcode
		 */
		static LvString ToRelativePathAtResourcesRelativePath(const char* relativePath);

		/**
		 * @brief 프로젝트 폴더의 상대경로를 통해 중복되지 않는 이름의 상대경로를 반환
		 * @param relativePath 대상의 프로젝트 폴더 기준 상대경로 ( "Resources/target.png" )
		 * @return 대상의 프로젝트 폴더 기준 겹치지 않는 상대경로 ( return "Resources/target_1.png"; )
		 * @details 이미 대상 경로가 존재하지 않는다면 그대로 반환, 대상 경로의 상위경로가 존재하지 않는 경우 빈 문자열 반환
		 */
		static LvString CreateUniqueRelativePath(const LvString& relativePath) { return CreateUniqueRelativePath(relativePath.c_str()); }

		/**
		 * @brief 프로젝트 폴더의 상대경로를 통해 중복되지 않는 이름의 상대경로를 반환
		 * @param relativePath 대상의 프로젝트 폴더 기준 상대경로 ( "Resources/target.png" )
		 * @return 대상의 프로젝트 폴더 기준 겹치지 않는 상대경로 ( return "Resources/target_1.png"; )
		 * @details 이미 대상 경로가 존재하지 않는다면 그대로 반환, 대상 경로의 상위경로가 존재하지 않는 경우 빈 문자열 반환
		 */
		static LvString CreateUniqueRelativePath(const char* relativePath);

		/**
		 * @brief 절대경로를 통해 중복되지 않는 이름의 절대경로를 반환
		 * @param absolutePath 절대경로 ( "C:/projects/test/Resources/target.png" )
		 * @return 대상의 겹치지 않는 절대경로 ( return "C:/projects/test/Resources/target_1.png"; )
		 * @details 이미 대상 경로가 존재하지 않는다면 그대로 반환, 대상 경로의 상위경로가 존재하지 않는 경우 빈 문자열 반환
		 */
		static LvString CreateUniqueAbsolutePath(const LvString& absolutePath) { return CreateUniqueAbsolutePath(absolutePath.c_str()); }

		/**
		 * @brief 절대경로를 통해 중복되지 않는 이름의 절대경로를 반환
		 * @param absolutePath 절대경로 ( "C:/projects/test/Resources/target.png" )
		 * @return 대상의 겹치지 않는 절대경로 ( return "C:/projects/test/Resources/target_1.png"; )
		 * @details 이미 대상 경로가 존재하지 않는다면 그대로 반환, 대상 경로의 상위경로가 존재하지 않는 경우 빈 문자열 반환
		 */
		static LvString CreateUniqueAbsolutePath(const char* absolutePath);
	};
}

LV_NS_EDITOR_END

#endif
