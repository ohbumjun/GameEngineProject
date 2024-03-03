#pragma once
#ifndef __LV_FILE_TABLE_H__
#define __LV_FILE_TABLE_H__

#include "editor/command/LvSerializePropertySet.h"
#include "system/LvString.h"
#include "system/LvHashtable.h"
#include "system/LvList.h"
#include "editor/private/project/LvFileInfo.h"

LV_NS_EDITOR_BEGIN
namespace Project
{
	class LvFileTable
	{
		/**
		 * \brief 에셋을 호환할 수 있도록 구성된 인포
		 */
		class Info
		{
		public:
			/**
			 * \param childName 찾을 파일 또는 디렉토리 이름
			 * \code
			 * fileInfo->FindChild("target.png")
			 * \endcode
			 */
			const Info* FindChild(const char* childName) const;

			/**
			 * \return 하위 파일 혹은 디렉토리의 갯수
			 */
			uint64 GetChildCount() const;

			/**
			 * \return 하위 LvFileInfo 반환
			 */
			const Info* GetChild(uint64 index) const;

			/**
			 * \return 하위 파일 혹은 디렉토리의 절대경로들을 반환
			 */
			const LvList<LvString>& GetChildAbsolutePaths() const;

			const LvFileInfo data;

			Info(Info&&) = delete;
			Info& operator = (Info&&) = delete;
			Info(const Info&) = delete;
			Info& operator = (const Info&) = delete;

		private:
			friend class LvFileTable;

			Info(LvFileTable* table, Info* parent, const char * absolutePath, LvList<LvString>* creates);
			~Info();

			Info* _parent;
			LvList<LvString> _childAbsolutePaths;
			LvFileTable* _table;
		};

	public:
		std::function <bool(const char* path)> isFileExist = nullptr; // 파일이 존재 확인.
		std::function <bool(const char* path)> isDirectoryExist = nullptr; // 디렉터리 유/무.
		std::function <LvString(const char* path)> getPathParent = nullptr;	// 부모 디렉토리 패스를 출력합니다.
		std::function <LvList<LvString>(const char* path)> getDirectoryLists = nullptr; // 디렉토리의 하위의 모든 파일과 디렉터리들을 리턴합니다.
		std::function <LvString(const char* path, const char* path2)> pathCombine = nullptr; // 두개의 경로를 경로 구분자와 함께 합성합니다.
		std::function <LvString(const char* path)> getPathName = nullptr; // path 의 파일명을 리턴합니다.

		LvFileTable() = default;
		~LvFileTable();

		LvFileTable(LvFileTable&&) = delete;
		LvFileTable& operator = (LvFileTable&&) = delete;
		LvFileTable(const LvFileTable&) = delete;
		LvFileTable& operator = (const LvFileTable&) = delete;

		const LvFileInfo& GetFileInfoAtAbsolutePath(const char* absolutePath) const;
		const LvList<LvString>& GetChildAtAbsolutePaths(const char* absolutePath) const;

		const LvFileInfo& CreateRoot(const char* rootAbsolutePath, const LvList<LvString>& targetPaths = {});
		bool RemoveRecord(const char* absolutePath, bool checkExistFile);
		bool MoveRecord(const char* oldAbsolutePath, const char* newAbsolutePath, LvList<LvString>* creates);
		bool CreateRecord(const char* absolutePath, LvList<LvString>* creates);
		bool ModifyRecord(const char* absolutePath, LvList<LvString>* creates);

		LvString ToAbsolutePath(const char* relativePath) const;
		LvString ToRelativePath(const char* absolutePath) const;

		Info* GetRoot() const { return _root; }

		void Clear();

	private:
		Info* createInstance(const char* parentAbsolutePath, const char * absolutePath, LvList<LvString>* creates);

		void regist(Info* info);
		void deregister(Info* info);
		void deregister(const char* absolutePath);

		bool isTargetPath(const char * absolutePath);
		
		void releaseFileInfo(Info * info);
		Info* getFileInfoAtAbsolutePath(const char* absolutePath) const;
		LvString toNormalize(const char* path) const;

		Info* _root = nullptr;
		LvString _rootAbsolutePath;
		LvList<LvString> _targetPaths;
		LvList<Info*> _infos;
		LvHashtable<LvString, Info*> _absolutePathToInfo;
	};
}

LV_NS_EDITOR_END

#endif 
