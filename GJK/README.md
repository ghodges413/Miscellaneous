# Gilbert-Johnson-Keerth

This is an implementation of the paper 
"A Fast Procedure for Computing the Distance Between Complex Objects in Three-Dimensional Space"

It uses the GJK to determine if an intersection occurs.  If it does then it returns a 3-simplex
containing the origin.  The expanding polytope algorithm (EPA) is then used to determine the distance
and intersection points.