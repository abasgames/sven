#pragma once
#include "address.hh"

template< typename tty >
class c_mini_hook {
public:
	__forceinline c_mini_hook( ) : m_addr{ }, original{ nullptr }, stored{ nullptr } { };
	__forceinline c_mini_hook( std::uintptr_t addr ) : m_addr{ addr } {
		original = m_addr.at< tty >( );
		stored = m_addr.at< tty >( );
	};

	__forceinline c_mini_hook( address addr ) {
		m_addr = addr;
		original = m_addr.at< tty >( );
		stored = m_addr.at< tty >( );
	};

	__forceinline void unhook( ) {
		m_addr.set< tty >( stored );
	};

	__forceinline void hook( tty func ) {
		function = func;
		m_addr.set< tty >( func );
	};

	__forceinline tty get( ) {
		return original;
	};
private:
	address m_addr;
	tty original, stored;
	tty function;
};