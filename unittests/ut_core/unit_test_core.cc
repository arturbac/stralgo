// SPDX-FileCopyrightText: 2024 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/stralgo
#include <unit_test_core.h>

std::ostream & operator<<(std::ostream & stream, non_trivial_ptr const & v)
  {
  if(v.value_)
    stream << *v.value_;
  else
    stream << "nullptr";
  return stream;
  }

std::ostream & operator<<(std::ostream & stream, non_trivial_ptr_except const & v)
  {
  if(v.obj.value_)
    stream << *v.obj.value_;
  else
    stream << "nullptr";
  return stream;
  }

std::ostream & operator<<(std::ostream & stream, non_trivial_ptr_except_copy const & v)
  {
  if(v.obj.value_)
    stream << *v.obj.value_;
  else
    stream << "nullptr";
  return stream;
  }
