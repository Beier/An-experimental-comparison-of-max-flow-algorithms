//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERPROCESS_SHM_NAMED_CONDITION_HPP
#define BOOST_INTERPROCESS_SHM_NAMED_CONDITION_HPP

#if (defined _MSC_VER) && (_MSC_VER >= 1200)
#  pragma once
#endif

#include "../../../interprocess/detail/config_begin.hpp"
#include "../../../interprocess/detail/workaround.hpp"
#include "../../../static_assert.hpp"
#include "../../../interprocess/detail/type_traits.hpp"
#include "../../../interprocess/creation_tags.hpp"
#include "../../../interprocess/exceptions.hpp"
#include "../../../interprocess/shared_memory_object.hpp"
#include "../../../interprocess/sync/interprocess_condition.hpp"
#include "../../../interprocess/detail/managed_open_or_create_impl.hpp"
#include "../../../interprocess/detail/posix_time_types_wrk.hpp"
#include "../../../interprocess/sync/shm/named_creation_functor.hpp"
#include "../../../interprocess/sync/named_mutex.hpp"
#include "../../../interprocess/permissions.hpp"
#if defined (BOOST_INTERPROCESS_NAMED_MUTEX_USES_POSIX_SEMAPHORES)
   #include "../../../interprocess/sync/interprocess_mutex.hpp"
   #include "../../../interprocess/sync/scoped_lock.hpp"
   #include "../../../interprocess/sync/detail/condition_any_algorithm.hpp"
#else
   #include "../../../interprocess/sync/detail/locks.hpp"
#endif


//!\file
//!Describes process-shared variables interprocess_condition class

namespace boost {
namespace interprocess {
namespace ipcdetail {

/// @cond
class interprocess_tester;
/// @endcond

//! A global condition variable that can be created by name.
//! This condition variable is designed to work with named_mutex and
//! can't be placed in shared memory or memory mapped files.
class shm_named_condition
{
   /// @cond
   //Non-copyable
   shm_named_condition();
   shm_named_condition(const shm_named_condition &);
   shm_named_condition &operator=(const shm_named_condition &);
   /// @endcond
   public:
   //!Creates a global condition with a name.
   //!If the condition can't be created throws interprocess_exception
   shm_named_condition(create_only_t create_only, const char *name, const permissions &perm = permissions());

   //!Opens or creates a global condition with a name.
   //!If the condition is created, this call is equivalent to
   //!shm_named_condition(create_only_t, ... )
   //!If the condition is already created, this call is equivalent
   //!shm_named_condition(open_only_t, ... )
   //!Does not throw
   shm_named_condition(open_or_create_t open_or_create, const char *name, const permissions &perm = permissions());

   //!Opens a global condition with a name if that condition is previously
   //!created. If it is not previously created this function throws
   //!interprocess_exception.
   shm_named_condition(open_only_t open_only, const char *name);

   //!Destroys *this and indicates that the calling process is finished using
   //!the resource. The destructor function will deallocate
   //!any system resources allocated by the system for use by this process for
   //!this resource. The resource can still be opened again calling
   //!the open constructor overload. To erase the resource from the system
   //!use remove().
   ~shm_named_condition();

   //!If there is a thread waiting on *this, change that
   //!thread's state to ready. Otherwise there is no effect.*/
   void notify_one();

   //!Change the state of all threads waiting on *this to ready.
   //!If there are no waiting threads, notify_all() has no effect.
   void notify_all();

   //!Releases the lock on the named_mutex object associated with lock, blocks
   //!the current thread of execution until readied by a call to
   //!this->notify_one() or this->notify_all(), and then reacquires the lock.
   template <typename L>
   void wait(L& lock);

   //!The same as:
   //!while (!pred()) wait(lock)
   template <typename L, typename Pr>
   void wait(L& lock, Pr pred);

   //!Releases the lock on the named_mutex object associated with lock, blocks
   //!the current thread of execution until readied by a call to
   //!this->notify_one() or this->notify_all(), or until time abs_time is reached,
   //!and then reacquires the lock.
   //!Returns: false if time abs_time is reached, otherwise true.
   template <typename L>
   bool timed_wait(L& lock, const boost::posix_time::ptime &abs_time);

   //!The same as:   while (!pred()) {
   //!                  if (!timed_wait(lock, abs_time)) return pred();
   //!               } return true;
   template <typename L, typename Pr>
   bool timed_wait(L& lock, const boost::posix_time::ptime &abs_time, Pr pred);

   //!Erases a named condition from the system.
   //!Returns false on error. Never throws.
   static bool remove(const char *name);

   /// @cond
   private:

   #if defined (BOOST_INTERPROCESS_NAMED_MUTEX_USES_POSIX_SEMAPHORES)
   class internal_condition_members
   {
      public:
      typedef interprocess_mutex       mutex_type;
      typedef interprocess_condition   condvar_type;
  
      condvar_type&  get_condvar() {  return m_cond;  }
      mutex_type&    get_mutex()   {  return m_mtx; }

      private:
      mutex_type     m_mtx;
      condvar_type   m_cond;
   };

   typedef ipcdetail::condition_any_wrapper<internal_condition_members> internal_condition;
   #else    //defined (BOOST_INTERPROCESS_NAMED_MUTEX_USES_POSIX_SEMAPHORES)
   typedef interprocess_condition internal_condition;
   #endif   //defined (BOOST_INTERPROCESS_NAMED_MUTEX_USES_POSIX_SEMAPHORES)

   internal_condition m_cond;

   friend class boost::interprocess::ipcdetail::interprocess_tester;
   void dont_close_on_destruction();

   typedef ipcdetail::managed_open_or_create_impl<shared_memory_object, 0, true, false> open_create_impl_t;
   open_create_impl_t m_shmem;

   template <class T, class Arg> friend class boost::interprocess::ipcdetail::named_creation_functor;
   typedef boost::interprocess::ipcdetail::named_creation_functor<internal_condition> construct_func_t;
   /// @endcond
};

/// @cond

inline shm_named_condition::~shm_named_condition()
{}

inline shm_named_condition::shm_named_condition(create_only_t, const char *name, const permissions &perm)
   :  m_shmem  (create_only
               ,name
               ,sizeof(internal_condition) +
                  open_create_impl_t::ManagedOpenOrCreateUserOffset
               ,read_write
               ,0
               ,construct_func_t(DoCreate)
               ,perm)
{}

inline shm_named_condition::shm_named_condition(open_or_create_t, const char *name, const permissions &perm)
   :  m_shmem  (open_or_create
               ,name
               ,sizeof(internal_condition) +
                  open_create_impl_t::ManagedOpenOrCreateUserOffset
               ,read_write
               ,0
               ,construct_func_t(DoOpenOrCreate)
               ,perm)
{}

inline shm_named_condition::shm_named_condition(open_only_t, const char *name)
   :  m_shmem  (open_only
               ,name
               ,read_write
               ,0
               ,construct_func_t(DoOpen))
{}

inline void shm_named_condition::dont_close_on_destruction()
{  interprocess_tester::dont_close_on_destruction(m_shmem);  }

#if defined(BOOST_INTERPROCESS_NAMED_MUTEX_USES_POSIX_SEMAPHORES)

inline void shm_named_condition::notify_one()
{  m_cond.notify_one(); }

inline void shm_named_condition::notify_all()
{  m_cond.notify_all(); }

template <typename L>
inline void shm_named_condition::wait(L& lock)
{  m_cond.wait(lock); }

template <typename L, typename Pr>
inline void shm_named_condition::wait(L& lock, Pr pred)
{  m_cond.wait(lock, pred); }

template <typename L>
inline bool shm_named_condition::timed_wait
   (L& lock, const boost::posix_time::ptime &abs_time)
{  return m_cond.timed_wait(lock, abs_time); }

template <typename L, typename Pr>
inline bool shm_named_condition::timed_wait
   (L& lock, const boost::posix_time::ptime &abs_time, Pr pred)
{  return m_cond.timed_wait(lock, abs_time, pred); }

#else

inline void shm_named_condition::notify_one()
{  m_cond.notify_one();  }

inline void shm_named_condition::notify_all()
{  m_cond.notify_all();  }

template <typename L>
inline void shm_named_condition::wait(L& lock)
{
   internal_mutex_lock<L> internal_lock(lock);
   m_cond.wait(internal_lock);
}

template <typename L, typename Pr>
inline void shm_named_condition::wait(L& lock, Pr pred)
{
   internal_mutex_lock<L> internal_lock(lock);
   m_cond.wait(internal_lock, pred);
}

template <typename L>
inline bool shm_named_condition::timed_wait
   (L& lock, const boost::posix_time::ptime &abs_time)
{
   internal_mutex_lock<L> internal_lock(lock);
   return m_cond.timed_wait(internal_lock, abs_time);
}

template <typename L, typename Pr>
inline bool shm_named_condition::timed_wait
   (L& lock, const boost::posix_time::ptime &abs_time, Pr pred)
{
   internal_mutex_lock<L> internal_lock(lock);
   return m_cond.timed_wait(internal_lock, abs_time, pred);
}

#endif

inline bool shm_named_condition::remove(const char *name)
{  return shared_memory_object::remove(name); }

/// @endcond

}  //namespace ipcdetail
}  //namespace interprocess
}  //namespace boost

#include "../../../interprocess/detail/config_end.hpp"

#endif // BOOST_INTERPROCESS_SHM_NAMED_CONDITION_HPP
