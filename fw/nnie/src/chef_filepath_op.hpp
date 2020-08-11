/**
 * @license  this file is a part of libchef. more info see https://github.com/q191201771/libchef
 * @tag      v1.10.17
 * @file     chef_filepath_op.hpp
 * @deps     nope
 * @platform linux | macos | xxx
 *
 * @author
 *   chef <191201771@qq.com>
 *     - initial release xxxx-xx-xx
 *
 * @brief    文件、文件夹常用操作帮助函数集合
 *
 */

#ifndef _CHEF_BASE_FILEPATH_OP_HPP_
#define _CHEF_BASE_FILEPATH_OP_HPP_
#pragma once

#include <string>
#include <vector>
#include <stdint.h>

namespace chef {

  class filepath_op {
    public:
      // @return 0 存在，文件或文件夹都可    -1 不存在
      static int exist(const std::string &name);

      // @return 0 存在，且为文件夹    -1 不存在，或不是文件夹
      static int is_dir(const std::string &pathname);

      // @return 0 是绝对路径格式（以`/`字符开头）    -1 不是绝对路径
      static int is_abs_path(const std::string &name);

      /**
       * @param pathname             需要查询的文件夹
       * @param child_dirs           传出参数，文件夹下的文件夹
       * @param child_files          传出参数，文件夹下的文件
       * @param with_pathname_prefix 传出的文件、文件夹前是否加上`pathname`前缀
       *
       * @return  0 成功 -1 失败 `pathname`不可遍历
       *
       * @NOTICE 只访问第一层目录，如果需要访问子目录下的内容，需要业务方自行递归
       *
       */
      static int walk_dir(const std::string &pathname,
                          std::vector<std::string> &child_dirs /*out*/,
                          std::vector<std::string> &child_files /*out*/,
                          bool with_pathname_prefix=true);

      // @return 0 创建成功，或创建前已经存在    -1 失败
      static int mkdir_recursive(const std::string &pathname);

      /**
       * @param name 文件名
       *
       * @return
       *    0 删除成功，或删除前就不存在
       *   -1 删除失败，或`name`是文件夹
       *
       */
      static int rm_file(const std::string &name);

      /**
       * @param pathname 文件夹名
       *
       * @return
       *    0 删除成功，或删除前就不存在
       *   -1 删除失败，或`pathname`不是文件夹
       *
       */
      static int rmdir_recursive(const std::string &pathname);

      /**
       * @param src 源文件
       * @param dst 目标文件
       *
       * @return 0 成功 -1 失败
       *
       */
      static int rename(const std::string &src, const std::string &dst);

      /**
       * 写文件
       *
       * @param append 当文件已经存在时，true则追加至末尾，false则覆盖原文件内容
       * @return 0 成功 -1 失败
       *
       */
      static int write_file(const std::string &filename, const std::string &content, bool append=false);

      /**
       * 写文件
       *
       * @param append 当文件已经存在时，true则追加至末尾，false则覆盖原文件内容
       * @return 0 成功 -1 失败
       *
       */
      static int write_file(const std::string &filename, const char *content, size_t content_size, bool append=false);

      /**
       * @NOTICE
       * For most files under the /proc directory, stat() does not return the file
       * size in the st_size field; instead the field is returned with the value 0.
       *
       * @return 文件大小，失败则返回-1
       *
       */
      static int64_t get_file_size(const std::string &filename);

      /**
       * 读文件，对get_file_size()+read_file()的封装，更易于使用
       *
       * @return 成功返回文件内容，失败返回std::string()
       *
       */
      static std::string read_file(const std::string &filename);

      /**
       * 由于/proc下面的文件无法通过::stat()获取文件长度，所以提供参数让调用者填入一个fixed长度
       *
       */
      static std::string read_file(const std::string &filename, size_t content_size);

      /**
       * @param filename     文件名
       * @param content      传出参数，读取到的文件内容，内存由外部申请
       * @param content_size 最大读入大小
       *
       * @return 成功返回实际读入大小，失败返回-1
       *
       */
      static int64_t read_file(const std::string &filename, char *content /*out*/, size_t content_size);

      /// @TODO 能否统一成一个接口，内部判断是否是否为link
      static std::string read_link(const std::string &filename, size_t content_size);

      /**
       * @param path     目录
       * @param filename 文件名
       *
       * 连接目录和文件名，解决`path`后面'/'和`filename`前面'/'是否存在，重复的问题
       *
       */
      static std::string join(const std::string &path, const std::string &filename);

    private:
      filepath_op();
      filepath_op(const filepath_op &);
      filepath_op &operator=(const filepath_op &);

  }; // class filepath_op

} // namespace chef





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// @NOTICE 该分隔线以上部分为该模块的接口，分割线以下部分为对应的实现





#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>

namespace chef {

#define IF_ZERO_RETURN_NAGETIVE_ONE(x) do { if ((x) == 0) return -1; } while(0);
#define IF_NULL_RETURN_NAGETIVE_ONE(x) do { if ((x) == NULL) return -1; } while(0);
#define IF_STRING_EMPTY_RETURN_NAGETIVE_ONE(x) do { if (x.empty()) return -1; } while(0);

  inline int filepath_op::exist(const std::string &name) {
    IF_STRING_EMPTY_RETURN_NAGETIVE_ONE(name);
    struct stat st;
    return stat(name.c_str(), &st);
  }

  inline int filepath_op::is_dir(const std::string &pathname) {
    IF_STRING_EMPTY_RETURN_NAGETIVE_ONE(pathname);
    struct stat st;
    if (stat(pathname.c_str(), &st) == -1) {
      return -1;
    }
    return S_ISDIR(st.st_mode) ? 0 : -1;
  }

  inline int filepath_op::is_abs_path(const std::string &name) {
    return !name.empty() && (name[0] == '/');
  }

  inline int filepath_op::walk_dir(const std::string &pathname,
                            std::vector<std::string> &child_dirs,
                            std::vector<std::string> &child_files,
                            bool with_pathname_prefix)
  {
    if (is_dir(pathname) == -1) {
      return -1;
    }

    child_dirs.clear();
    child_files.clear();

    DIR *open_ret = ::opendir(pathname.c_str());
    IF_NULL_RETURN_NAGETIVE_ONE(open_ret);

    struct dirent entry;
    struct dirent *result = NULL;
    for (;;) {
      if (::readdir_r(open_ret, &entry, &result) != 0) {
        break;
      }
      if (result == NULL) {
        break;
      }
      char *name = result->d_name;
      if (strcmp(name, ".") == 0 ||
          strcmp(name, "..") == 0
      ) {
        continue;
      }
      std::string file_with_path = filepath_op::join(pathname, name);
      if (filepath_op::exist(file_with_path.c_str()) != 0) {
        fprintf(stderr, "%s:%d %s\n", __FUNCTION__, __LINE__, file_with_path.c_str());
        continue;
      }
      if (filepath_op::is_dir(file_with_path.c_str()) == 0) {
        child_dirs.push_back(with_pathname_prefix ? file_with_path : name);
      } else {
        child_files.push_back(with_pathname_prefix ? file_with_path : name);
      }
    }

    if (open_ret) {
      ::closedir(open_ret);
    }

    return 0;
  }

  inline int filepath_op::mkdir_recursive(const std::string &pathname) {
    IF_STRING_EMPTY_RETURN_NAGETIVE_ONE(pathname);
    char *path_dup = strdup(pathname.c_str());
    size_t len = strlen(path_dup);
    if (len == 0) {
      return -1;
    }
    size_t i = path_dup[0] == '/' ? 1 : 0;
    for (; i <= len; ++i) {
      if (path_dup[i] == '/' || path_dup[i] == '\0') {
        char ch = path_dup[i];
        path_dup[i] = '\0';
        if (::mkdir(path_dup, 0755) == -1 && errno != EEXIST) {
          free(path_dup);
          return -1;
        }
        path_dup[i] = ch;
      }
    }
    free(path_dup);
    return 0;
  }

  inline int filepath_op::rm_file(const std::string &name) {
    IF_STRING_EMPTY_RETURN_NAGETIVE_ONE(name);
    if (exist(name) == -1) {
      return 0;
    }
    if (is_dir(name) == 0) {
      return -1;
    }
    if (::unlink(name.c_str()) == -1) {
      return -1;
    }
    return 0;
  }

  inline int filepath_op::rmdir_recursive(const std::string &pathname) {
    IF_STRING_EMPTY_RETURN_NAGETIVE_ONE(pathname);
    if (exist(pathname) == -1) {
      return 0;
    }
    if (is_dir(pathname) == -1) {
      return -1;
    }

    DIR *open_ret = ::opendir(pathname.c_str());
    IF_NULL_RETURN_NAGETIVE_ONE(open_ret);

    struct dirent entry;
    struct dirent *result = NULL;
    int ret = 0;
    for (;;) {
      if (::readdir_r(open_ret, &entry, &result) != 0) {
        break;
      }
      if (result == NULL) {
        break;
      }
      char *name = result->d_name;
      if (strcmp(name, ".") == 0 ||
          strcmp(name, "..") == 0
      ) {
        continue;
      }
      std::string file_with_path = filepath_op::join(pathname, name);
      if (filepath_op::exist(file_with_path.c_str()) != 0) {
        fprintf(stderr, "%s:%d %s\n", __FUNCTION__, __LINE__, file_with_path.c_str());
        continue;
      }
      if (filepath_op::is_dir(file_with_path.c_str()) == 0) {
        if (filepath_op::rmdir_recursive(file_with_path.c_str()) != 0) {
          ret = -1;
        }
      } else {
        if (filepath_op::rm_file(file_with_path.c_str()) != 0) {
          ret = -1;
        }
      }
    }

    if (open_ret) {
      ::closedir(open_ret);
    }

    return (::rmdir(pathname.c_str()) == 0 && ret == 0) ? 0 : -1;
  }

  inline int filepath_op::rename(const std::string &src, const std::string &dst) {
    IF_STRING_EMPTY_RETURN_NAGETIVE_ONE(src);
    IF_STRING_EMPTY_RETURN_NAGETIVE_ONE(dst);
    return ::rename(src.c_str(), dst.c_str());
  }

  inline int filepath_op::write_file(const std::string &filename, const char *content, size_t content_size, bool append) {
    IF_STRING_EMPTY_RETURN_NAGETIVE_ONE(filename);
    IF_NULL_RETURN_NAGETIVE_ONE(content);
    IF_ZERO_RETURN_NAGETIVE_ONE(content_size);
    FILE *fp = fopen(filename.c_str(), append ? "ab" : "wb");
    IF_NULL_RETURN_NAGETIVE_ONE(fp);
    size_t written = fwrite(reinterpret_cast<const void *>(content), 1, content_size, fp);
    fclose(fp);
    return (written == content_size) ? 0 : -1;
  }

  inline int filepath_op::write_file(const std::string &filename, const std::string &content, bool append) {
    return filepath_op::write_file(filename, content.c_str(), content.length(), append);
  }

  inline int64_t filepath_op::get_file_size(const std::string &filename) {
    IF_STRING_EMPTY_RETURN_NAGETIVE_ONE(filename);
    if (exist(filename) == -1 || is_dir(filename) == 0) {
      return -1;
    }
    struct stat st;
    if (::stat(filename.c_str(), &st) == -1) {
      return -1;
    }
    return st.st_size;
  }

  inline int64_t filepath_op::read_file(const std::string &filename, char *content, size_t content_size) {
    IF_STRING_EMPTY_RETURN_NAGETIVE_ONE(filename);
    IF_NULL_RETURN_NAGETIVE_ONE(content);
    IF_ZERO_RETURN_NAGETIVE_ONE(content_size);
    FILE *fp = fopen(filename.c_str(), "rb");
    IF_NULL_RETURN_NAGETIVE_ONE(fp);
    size_t read_size = fread(reinterpret_cast<void *>(content), 1, content_size, fp);
    fclose(fp);
    return read_size;
  }

  inline std::string filepath_op::read_file(const std::string &filename) {
    int64_t size = get_file_size(filename);
    if (size <= 0) {
      return std::string();
    }
    return read_file(filename, size);
  }

  inline std::string filepath_op::read_file(const std::string &filename, size_t content_size) {
    if (content_size == 0) {
      return std::string();
    }
    char *content = new char[content_size];
    int64_t read_size = read_file(filename.c_str(), content, content_size);
    if (read_size == -1) {
      delete []content;
      return std::string();
    }
    std::string content_string(content, read_size);
    delete []content;
    return content_string;
  }

  inline std::string filepath_op::read_link(const std::string &filename, size_t content_size) {
    if (filename.empty() || content_size == 0) {
      return std::string();
    }
    char *content = new char[content_size];
    ssize_t length = ::readlink(filename.c_str(), content, content_size);
    if (length == -1) {
      delete []content;
      return std::string();
    }
    std::string ret(content, length);
    delete []content;
    return ret;
  }

  inline std::string filepath_op::join(const std::string &path, const std::string &filename) {
    std::string ret;
    size_t path_length = path.length();
    size_t filename_length = filename.length();
    if (path_length == 0) {
      return filename;
    }
    if (filename_length == 0) {
      return path;
    }
    if (path[path_length - 1] == '/') {
      ret = path.substr(0, path_length - 1);
    } else {
      ret = path;
    }
    ret += "/";
    if (filename[0] == '/') {
      ret += filename.substr(1, filename_length - 1);
    } else {
      ret += filename;
    }
    return ret;
  }

#undef IF_ZERO_RETURN_NAGETIVE_ONE
#undef IF_NULL_RETURN_NAGETIVE_ONE
#undef IF_STRING_EMPTY_RETURN_NAGETIVE_ONE

} // namespace chef

#endif // _CHEF_BASE_FILEPATH_OP_HPP_
