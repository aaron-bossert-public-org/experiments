
#pragma once

#include "glm/geometric.hpp"
#include "glm/mat4x4.hpp"

namespace utility
{
	inline aabb make_aabb( const glm::vec3* vertices, size_t count )
	{
		aabb a;

		if ( count )
		{
			a.min = a.max = *vertices;
		}


		for ( int32_t i = 1; i < count; ++i )
		{
			const glm::vec3& vertex = vertices[i];
			a.min = glm::min( a.min, vertex );
			a.max = glm::max( a.max, vertex );
		}

		return a;
	}

	inline glm::vec3 center( const aabb& a )
	{
		return ( a.min + a.max ) / 2.f;
	}

	inline glm::vec3 dimensions( const aabb& a )
	{
		return a.max - a.min;
	}

	inline glm::vec3 radii( const aabb& a )
	{
		return dimensions( a ) / 2.f;
	}

	inline float dist_plane(
		const glm::vec4& normalized_plane,
		glm::vec3 point )
	{
		return normalized_plane.x * point.x + normalized_plane.y * point.y +
			normalized_plane.z * point.z + normalized_plane.w;
	}

	inline aabb move_bounds( const aabb& a, const glm::vec3& d )
	{
		aabb r = {
			a.min + d,
			a.max - d,
		};
		glm::vec3 c = center( a );
		r.min = glm::min( r.min, c );
		r.max = glm::max( r.max, c );
		return a;
	}

	inline aabb move_bounds( const aabb& a, const float dr )
	{
		return move_bounds(
			a,
			glm::vec3{
				dr,
				dr,
				dr,
			} );
	}

	inline glm::vec3 clamp( const aabb& a, const glm::vec3& p )
	{
		return {
			glm::clamp( p.x, a.min.x, a.max.x ),
			glm::clamp( p.y, a.min.y, a.max.y ),
			glm::clamp( p.z, a.min.z, a.max.z ),
		};
	}

	inline bool contains( const aabb& a, const glm::vec3& p )
	{
		return ( p.x >= a.min.x && p.x <= a.max.x ) &&
			( p.y >= a.min.y && p.y <= a.max.y ) &&
			( p.z >= a.min.z && p.z <= a.max.z );
	}

	inline bool contains( const aabb& a, const aabb& other )
	{
		return contains( a, other.min ) && contains( a, other.max );
	}

	inline bool intersects( const aabb& a, const aabb& other )
	{
		return ( a.min.x <= other.max.x && a.max.x >= other.min.x ) &&
			( a.min.y <= other.max.y && a.max.y >= other.min.y ) &&
			( a.min.z <= other.max.z && a.max.z >= other.min.z );
	}

	inline bool intersects( const aabb& a, const sphere& s )
	{
		const glm::vec3 p = clamp( a, s.center );
		const float d = glm::distance( p, s.center );

		return d <= s.radius;
	}

	inline aabb combine( const aabb& a, const aabb& b )
	{
		return {
			glm::min( a.min, b.min ),
			glm::max( a.max, b.max ),
		};
	}

	//------------------------------------------------------------------------------

	inline bool intersects( const frustum& norm_frust, const sphere& sphere )
	{
		/* approximate: this does not accurately detect intersection
		 * a small percentage of comparisons will return true despite
		 * the shapes not intersecting.
		 */
		if ( dist_plane( norm_frust.left, sphere.center ) < -sphere.radius )
			return false;
		if ( dist_plane( norm_frust.right, sphere.center ) < -sphere.radius )
			return false;
		if ( dist_plane( norm_frust.bottom, sphere.center ) < -sphere.radius )
			return false;
		if ( dist_plane( norm_frust.top, sphere.center ) < -sphere.radius )
			return false;
		if ( dist_plane( norm_frust.near, sphere.center ) < -sphere.radius )
			return false;
		if ( dist_plane( norm_frust.far, sphere.center ) < -sphere.radius )
			return false;

		return true;
	}

	inline aabb translate( const aabb& a, const glm::vec3& dp )
	{
		return {
			a.min + dp,
			a.max + dp,
		};
	}

	inline aabb transform( const aabb& a, const glm::mat4x4& matrix )
	{
		const glm::vec3& o = a.min;
		const glm::vec3& i = a.max;

		glm::vec3 vertices[] = {
			{ o.x, o.y, o.z },
			{ o.x, o.y, i.z },
			{ o.x, i.y, o.z },
			{ o.x, i.y, i.z },

			{ i.x, o.y, o.z },
			{ i.x, o.y, i.z },
			{ i.x, i.y, o.z },
			{ i.x, i.y, i.z },
		};

		for ( auto& vertex : vertices )
		{
			vertex = matrix * glm::vec4( vertex, 1.f );
		}

		return make_aabb( vertices, sizeof( vertices ) / sizeof( *vertices ) );
	}


	//------------------------------------------------------------------------------

	inline glm::vec4 norm_plane( const glm::vec4& rhs )
	{
		glm::vec3 n( rhs.x, rhs.y, rhs.z );
		return rhs / glm::length( n );
	}

	inline frustum normalize( const frustum& rhs )
	{
		return {
			norm_plane( rhs.left ),
			norm_plane( rhs.right ),
			norm_plane( rhs.bottom ),
			norm_plane( rhs.top ),
			norm_plane( rhs.near ),
			norm_plane( rhs.far ),
		};
	}

	//------------------------------------------------------------------------------

	inline glm::vec4 make_plane( const glm::vec3& p, const glm::vec3& n )
	{
		return {
			n,
			-glm::dot( n, p ),
		};
	}

	inline glm::vec4 make_plane(
		const glm::vec3& p0,
		const glm::vec3& p1,
		const glm::vec3& p2 )
	{
		return make_plane(
			p0,
			glm::normalize( glm::cross( p1 - p0, p2 - p0 ) ) );
	}

	inline frustum make_frustum( const glm::mat4x4& m )
	{
		return normalize( frustum{
			// left
			glm::vec4(
				m[0][3] + m[0][0],
				m[1][3] + m[1][0],
				m[2][3] + m[2][0],
				m[3][3] + m[3][0] ),
			// right
			glm::vec4(
				m[0][3] - m[0][0],
				m[1][3] - m[1][0],
				m[2][3] - m[2][0],
				m[3][3] - m[3][0] ),
			// bottom
			glm::vec4(
				m[0][3] + m[0][1],
				m[1][3] + m[1][1],
				m[2][3] + m[2][1],
				m[3][3] + m[3][1] ),
			// top
			glm::vec4(
				m[0][3] - m[0][1],
				m[1][3] - m[1][1],
				m[2][3] - m[2][1],
				m[3][3] - m[3][1] ),
			// near
			glm::vec4(
				m[0][3] + m[0][2],
				m[1][3] + m[1][2],
				m[2][3] + m[2][2],
				m[3][3] + m[3][2] ),
			// far
			glm::vec4(
				m[0][3] - m[0][2],
				m[1][3] - m[1][2],
				m[2][3] - m[2][2],
				m[3][3] - m[3][2] ),
		} );
	}
}