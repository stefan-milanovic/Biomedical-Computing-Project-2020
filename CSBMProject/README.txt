The values in the configuration files need to adhere to the following rules:

	1) The field "SimulationType" can only have one of the following values:
		SIR, SEIR, SEIR_simplified.

		Any value other than this one will cause the program to exit immediately.

	2) The "lower_bound" and "upper_bound" fields must have a value according to the parent object
		(eg. for the "population" object these values are integers and for the "parameters" objects the
		values are doubles that are between 0 and 1).