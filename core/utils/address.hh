#pragma once
#include <cstdint>
#include <cstddef>
class address {
public:
	__forceinline address( ) : m_addr{} {}

	__forceinline address( uintptr_t addr ) : m_addr{ addr } {};

	__forceinline address( const void* ptr ) : m_addr{ reinterpret_cast< std::uintptr_t >( ptr ) } { };

	__forceinline operator std::uintptr_t( ) { return m_addr; };

	__forceinline operator void* ( ) { return reinterpret_cast< void* >( m_addr ); };

	__forceinline operator const void* ( ) { return reinterpret_cast< const void* >( m_addr ); };

	__forceinline bool operator==( address e ) const {
		return as< std::uintptr_t >( ) == e.as< std::uintptr_t >( );
	};

	__forceinline bool operator!=( address e ) const {
		return as< std::uintptr_t >( ) != e.as< std::uintptr_t >( );
	};

	__forceinline bool operator>( address e ) const {
		return as< std::uintptr_t >( ) > e.as< std::uintptr_t >( );
	};

	__forceinline bool operator<( address e ) const {
		return as< std::uintptr_t >( ) < e.as< std::uintptr_t >( );
	};

	__forceinline bool operator>=( address e ) const {
		return as< std::uintptr_t >( ) >= e.as< std::uintptr_t >( );
	};

	__forceinline bool operator<=( address e ) const {
		return as< std::uintptr_t >( ) <= e.as< std::uintptr_t >( );
	};

	__forceinline address operator+( address e ) const {
		return as< std::uintptr_t >( ) + e.as< std::uintptr_t >( );
	};

	__forceinline address operator-( address e ) const {
		return as< std::uintptr_t >( ) - e.as< std::uintptr_t >( );
	};

	template< typename tty = address >
	__forceinline tty as( ) const {
		return ( m_addr ) ? ( tty )( m_addr ) : tty{ };
	};

	template< typename tty = address >
	__forceinline tty as( std::size_t offset ) const {
		return ( m_addr ) ? ( tty )( m_addr + offset ) : tty{ };
	};

	template< typename tty = address >
	__forceinline tty as( std::ptrdiff_t offset ) const {
		return ( m_addr ) ? ( tty )( m_addr + offset ) : tty{ };
	};

	template< typename tty = address >
	__forceinline tty at( void ) const {
		return ( m_addr ) ? *reinterpret_cast< tty* >( m_addr ) : tty{};
	};

	template< typename tty = address >
	__forceinline tty at( std::size_t offset ) const {
		return ( m_addr ) ? *reinterpret_cast< tty* >( m_addr + offset ) : tty{};
	};

	template< typename tty = address >
	__forceinline tty at( std::ptrdiff_t offset ) const {
		return ( m_addr ) ? *reinterpret_cast< tty* >( m_addr + offset ) : tty{};
	};

	template< typename tty = address >
	__forceinline tty* ptr( std::ptrdiff_t offset ) const {
		return ( m_addr ) ? *reinterpret_cast< tty** >( m_addr + offset ) : tty{};
	};

	template< typename tty = address >
	__forceinline tty* ptr( void ) const {
		return ( m_addr ) ? *reinterpret_cast< tty** >( m_addr ) : nullptr;
	};

	template< typename tty = address >
	__forceinline tty add( std::size_t offset ) {
		return ( m_addr ) ? static_cast< tty >( m_addr + offset ) : tty{};
	};

	template< typename tty = address >
	__forceinline tty add( std::ptrdiff_t offset ) {
		return ( m_addr ) ? static_cast< tty >( m_addr + offset ) : tty{};
	};

	template< typename tty = address >
	__forceinline tty sub( std::size_t offset ) {
		return ( m_addr ) ? static_cast< tty >( m_addr - offset ) : tty{};
	};

	template< typename tty = address >
	__forceinline tty sub( std::ptrdiff_t offset ) {
		return ( m_addr ) ? static_cast< tty >( m_addr - offset ) : tty{};
	};

	template< typename tty = address >
	__forceinline tty mult( std::size_t offset ) {
		return ( m_addr ) ? static_cast< tty >( m_addr * offset ) : tty{};
	};

	template< typename tty = address >
	__forceinline tty mult( std::ptrdiff_t offset ) {
		return ( m_addr ) ? static_cast< tty >( m_addr * offset ) : tty{};
	};

	template< typename tty = address >
	__forceinline tty rva( address base, std::size_t offset ) {
		return base.as< tty >( offset );
	};

	template< typename tty = address >
	__forceinline tty rva( address base, std::ptrdiff_t offset ) {
		return base.as< tty >( offset );
	};

	template< typename tty = address >
	__forceinline tty rva( address to ) {
		return at< tty >( to );
	};

	template< typename tty = address >
	__forceinline void set( tty value ) {
		*reinterpret_cast< tty* >( m_addr ) = value;
	};
private:
	std::uintptr_t m_addr;
	DWORD dw_old_protection;
};