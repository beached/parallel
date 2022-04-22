// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/parallel
//

#pragma once

#include "daw_condition_variable.h"

#include <daw/cpp_17.h>
#include <daw/daw_cpp_feature_check.h>
#include <daw/daw_exception.h>
#include <daw/daw_move.h>

#include <atomic>
#include <cassert>
#include <ciso646>
#include <cstdint>
#include <memory>

namespace daw {
	template<typename>
	inline constexpr bool is_latch_v = false;

	template<typename>
	inline constexpr bool is_unique_latch_v = false;

	template<typename>
	inline constexpr bool is_shared_latch_v = false;

	template<typename Mutex, typename ConditionVariable>
	class basic_latch {
		mutable daw::basic_condition_variable<Mutex, ConditionVariable> m_condition =
		  daw::basic_condition_variable<Mutex, ConditionVariable>( );
		std::atomic_intmax_t m_count = 1;

		[[nodiscard]] auto stop_waiting( ) const {
			return [&]( ) -> bool { return static_cast<intmax_t>( m_count ) <= 0; };
		}

		void decrement( ) {
			--m_count;
		}

	public:
		basic_latch( ) = default;

		template<
		  typename Integer,
		  std::enable_if_t<std::is_integral_v<daw::remove_cvref_t<Integer>>, std::nullptr_t> = nullptr>
		explicit basic_latch( Integer count ) noexcept(
		  std::is_nothrow_default_constructible_v<std::atomic_intmax_t>
		    and std::is_nothrow_default_constructible_v<ConditionVariable> )
		  : m_count( static_cast<intmax_t>( count ) ) {}

		template<
		  typename Integer,
		  std::enable_if_t<std::is_integral_v<daw::remove_cvref_t<Integer>>, std::nullptr_t> = nullptr>
		basic_latch( Integer count, bool latched ) noexcept(
		  std::is_nothrow_default_constructible_v<std::atomic_intmax_t>
		    and std::is_nothrow_default_constructible_v<ConditionVariable> )
		  : m_count( static_cast<intmax_t>( count ) ) {}

		void reset( ) {
			m_count = 1;
		}

		template<
		  typename Integer,
		  std::enable_if_t<std::is_integral_v<daw::remove_cvref_t<Integer>>, std::nullptr_t> = nullptr>
		void reset( Integer count ) {

			m_count = static_cast<intmax_t>( count );
		}

		void add_notifier( ) {
			++m_count;
		}

		void notify( ) {
			decrement( );
			m_condition.notify_all( );
		}

		void notify_one( ) {
			decrement( );
			m_condition.notify_one( );
		}

		void wait( ) const {
			// Try a spin before we use the heavy guns
			for( size_t n = 0; n < 100; ++n ) {
				if( try_wait( ) ) {
					return;
				}
			}
			m_condition.wait( stop_waiting( ) );
		}

		[[nodiscard]] bool try_wait( ) const {
			return stop_waiting( )( );
		}

		template<typename Rep, typename Period>
		[[nodiscard]] auto wait_for( std::chrono::duration<Rep, Period> const &rel_time ) {
			return m_condition.wait_for( rel_time, stop_waiting( ) );
		}

		template<typename Clock, typename Duration>
		[[nodiscard]] auto wait_until( std::chrono::time_point<Clock, Duration> const &timeout_time ) {
			return m_condition.wait_until( timeout_time, stop_waiting( ) );
		}
	}; // basic_latch

	template<typename Mutex, typename ConditionVariable>
	inline constexpr bool is_latch_v<basic_latch<Mutex, ConditionVariable>> = true;

	using latch = basic_latch<std::mutex, std::condition_variable>;

	template<typename Mutex, typename ConditionVariable>
	class basic_unique_latch {
		using latch_t = basic_latch<Mutex, ConditionVariable>;
		std::unique_ptr<latch_t> latch = std::make_unique<latch_t>( );

	public:
		constexpr basic_unique_latch( ) = default;

		template<
		  typename Integer,
		  std::enable_if_t<std::is_integral_v<daw::remove_cvref_t<Integer>>, std::nullptr_t> = nullptr>
		explicit basic_unique_latch( Integer count ) noexcept(
		  std::is_nothrow_default_constructible_v<std::atomic_intmax_t>
		    and std::is_nothrow_default_constructible_v<ConditionVariable> )
		  : latch( std::make_unique<latch_t>( count ) ) {}

		template<
		  typename Integer,
		  std::enable_if_t<std::is_integral_v<daw::remove_cvref_t<Integer>>, std::nullptr_t> = nullptr>
		basic_unique_latch( Integer count, bool latched ) noexcept(
		  std::is_nothrow_default_constructible_v<std::atomic_intmax_t>
		    and std::is_nothrow_default_constructible_v<ConditionVariable> )
		  : latch( std::make_unique<latch_t>( count, latched ) ) {}

		basic_latch<Mutex, ConditionVariable> *release( ) {
			return latch.release( );
		}

		void add_notifier( ) {
			assert( latch );
			latch->add_notifier( );
		}

		void notify( ) {
			assert( latch );
			latch->notify( );
		}

		void set_latch( ) {
			assert( latch );
			latch->set_latch( );
		}

		void wait( ) const {
			assert( latch );
			latch->wait( );
		}

		[[nodiscard]] bool try_wait( ) const {
			assert( latch );
			return latch->try_wait( );
		}

		template<typename Rep, typename Period>
		[[nodiscard]] decltype( auto )
		wait_for( std::chrono::duration<Rep, Period> const &rel_time ) const {
			assert( latch );
			return latch->wait_for( rel_time );
		}

		template<typename Clock, typename Duration>
		[[nodiscard]] decltype( auto )
		wait_until( std::chrono::time_point<Clock, Duration> const &timeout_time ) {
			assert( latch );
			return latch->wait_until( timeout_time );
		}

		[[nodiscard]] explicit operator bool( ) const noexcept {
			return static_cast<bool>( latch );
		}
	}; // basic_unique_latch

	template<typename Mutex, typename ConditionVariable>
	inline constexpr bool is_unique_latch_v<basic_unique_latch<Mutex, ConditionVariable>> = true;

	using unique_latch = basic_unique_latch<std::mutex, std::condition_variable>;

	template<typename Mutex, typename ConditionVariable>
	class basic_shared_latch {
		using latch_t = basic_latch<Mutex, ConditionVariable>;
		std::shared_ptr<latch_t> latch = std::make_shared<latch_t>( );

	public:
		basic_shared_latch( ) = default;

		template<
		  typename Integer,
		  std::enable_if_t<std::is_integral_v<daw::remove_cvref_t<Integer>>, std::nullptr_t> = nullptr>
		explicit basic_shared_latch( Integer count ) noexcept(
		  std::is_nothrow_default_constructible_v<std::atomic_intmax_t>
		    and std::is_nothrow_default_constructible_v<ConditionVariable> )
		  : latch( std::make_shared<latch_t>( count ) ) {}

		template<
		  typename Integer,
		  std::enable_if_t<std::is_integral_v<daw::remove_cvref_t<Integer>>, std::nullptr_t> = nullptr>
		basic_shared_latch( Integer count, bool latched ) noexcept(
		  std::is_nothrow_default_constructible_v<std::atomic_intmax_t>
		    and std::is_nothrow_default_constructible_v<ConditionVariable> )
		  : latch( std::make_shared<latch_t>( count, latched ) ) {}

		explicit basic_shared_latch( basic_unique_latch<Mutex, ConditionVariable> &&other ) noexcept
		  : latch( other.release( ) ) {}

		void add_notifier( ) {
			assert( latch );
			latch->add_notifier( );
		}

		void notify( ) {
			assert( latch );
			latch->notify( );
		}

		void set_latch( ) {
			assert( latch );
			latch->notify( );
		}

		void wait( ) const {
			assert( latch );
			latch->wait( );
		}

		[[nodiscard]] bool try_wait( ) const {
			assert( latch );
			return latch->try_wait( );
		}

		template<typename Rep, typename Period>
		[[nodiscard]] decltype( auto )
		wait_for( std::chrono::duration<Rep, Period> const &rel_time ) const {
			assert( latch );
			return latch->wait_for( rel_time );
		}

		template<typename Clock, typename Duration>
		[[nodiscard]] decltype( auto )
		wait_until( std::chrono::time_point<Clock, Duration> const &timeout_time ) {
			assert( latch );
			return latch->wait_until( timeout_time );
		}

		[[nodiscard]] explicit operator bool( ) const noexcept {
			return static_cast<bool>( latch );
		}
	}; // basic_shared_latch

	template<typename Mutex, typename ConditionVariable>
	inline constexpr bool is_shared_latch_v<basic_shared_latch<Mutex, ConditionVariable>> = true;

	using shared_latch = basic_shared_latch<std::mutex, std::condition_variable>;

	template<typename Mutex, typename ConditionVariable>
	void wait_all( std::initializer_list<basic_latch<Mutex, ConditionVariable>> semaphores ) {
		for( auto &sem : semaphores ) {
			sem->wait( );
		}
	}

	template<typename T>
	struct is_latch : std::bool_constant<is_latch_v<T>> {};

	template<typename T>
	struct is_unique_latch : std::bool_constant<is_unique_latch_v<T>> {};

	template<typename T>
	struct is_shared_latch : std::bool_constant<is_shared_latch_v<T>> {};

#if defined( __cpp_concepts )
#if __cpp_concepts >= 201907L
	template<typename T>
	concept Latch = is_latch_v<T>;

	template<typename T>
	concept UniqueLatch = is_unique_latch_v<T>;

	template<typename T>
	concept SharedLatch = is_shared_latch_v<T>;
#endif
#endif

} // namespace daw
