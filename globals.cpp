




double map( double value, double in_min, double in_max, double out_min, double out_max )
{
	if		( value > in_max ) { value = in_max; }
	else if ( value < in_min ) { value = in_min; }

	return (  out_min + ( value - in_min ) * ( (out_max - out_min)/(in_max - in_min) )  );  
}
