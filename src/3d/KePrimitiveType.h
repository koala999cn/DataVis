#pragma once


enum KePrimitiveType
{
	// Draws a point at each of the n vertices
	k_points, 

	//  Draws a series of unconnected line segments. Segments are drawn between v0 and v1,
	// between v2 and v3, and so on.If n is odd, the last segment is drawn between vn每3and vn每2, and vn每1 is ignored.
	k_lines, 
	
	// Draws a line segment from v0 to v1, then from v1 to v2, and so on, finally drawing the segment
	// from vn每2 to vn每1.Thus, a total of n每1 line segments are drawn.Nothing is drawn unless n
	// is larger than 1. There are no restrictions on the vertices describing a line strip(or a line loop); the lines can intersect arbitrarily.
	k_line_strip, 
		            
	//  Same as k_line_strip, except that a final line segment is drawn from vn每1 to v0, completing a loop.        
	k_line_loop, 

	// Draws a series of triangles (three-sided polygons) using vertices v0, v1, v2, then v3, v4, v5, and so on.
	// If n isn＊t a multiple of 3, the final one or two vertices are ignored
	k_triangles, 
	
	// Draws a series of triangles (three-sided polygons) using vertices v0, v1, v2, then v2, v1, v3(note the order), then v2, v3, v4, and so on.
	// The ordering is to ensure that the triangles are all drawn with the same orientation so that
	// the strip can correctly form part of a surface.
	// Preserving the orientation is important for some operations, such as culling.
	// n must be at least 3 for anything to be drawn.
	k_triangle_strip, 
		                
	// Same as k_triangle_strip, except that the vertices are v0, v1, v2, then v0, v2, v3, then v0, v3, v4, and so on              			
	k_triangle_fan, 

	// Draws a series of quadrilaterals (four-sided polygons) using vertices v0, v1, v2, v3, 
	// then v4, v5, v6, v7, and so on.If n isn＊t a multiple of 4, the final one, two, or three vertices are ignored.
	k_quads, 
	
	// Draws a series of quadrilaterals (four-sided polygons) beginning with v0, v1, v3, v2, 
	// then v2, v3, v5, v4, then v4, v5, v7, v6, and so on
	// n must be at least 4 before anything is drawn.If n is odd, the final vertex is ignored.
	k_quad_strip, 
		            
	// Draws a polygon using the points v0, ... , vn每1 as vertices.	
	// n must be at least 3, or nothing is drawn.In addition, the polygon specified must
	// not intersect itselfand must be convex. If the vertices don＊t satisfy these conditions, the
	// results are unpredictable.
	k_polygon 	        
};