%typemap(in) (AJ_Message* msg, const char* signature, ...) {
  
  void *argp1 = 0 ;
  int res1 = 0 ;

	AJ_Arg arg;
	AJ_Status _status;

  res1 = SWIG_ConvertPtr(obj0, &argp1, SWIGTYPE_p__AJ_Message, 0 |  0 );
  if (!SWIG_IsOK(res1)) {
    SWIG_exception_fail(SWIG_ArgError(res1), "in method '" "AJ_(Un)MarshalArgs" "', argument " "1"" of type '" "AJ_Message *""'"); 
  }
  arg1 = (AJ_Message *)(argp1);

	printf("Called (Un)MarshalArgs.\n");
	printf("Argument Counts: %d %d\n", (int)PyTuple_Size(args), (int)PyTuple_Size(varargs));
	printf("Arguments: %s %s %s %s\n", PyString_AsString(PyObject_Repr(PyTuple_GetItem(args, 0))), PyString_AsString(PyTuple_GetItem(varargs, 0)), PyString_AsString(PyTuple_GetItem(varargs, 1)), PyString_AsString(PyTuple_GetItem(varargs, 2)));

	int argc, i, sig_len;
	char *signature, *cdata;

	argc = PyTuple_Size(varargs)-1;
	signature = PyString_AsString(PyTuple_GetItem(varargs, 0));
	sig_len = PyString_Size(PyTuple_GetItem(varargs, 0));

	if (argc != sig_len) {
		PyErr_SetString(PyExc_ValueError, "Wrong number of arguments for format string.");
		return NULL;
	}

	for (i = 0; i < argc; i++) {
		PyObject *o = PyTuple_GetItem(varargs, i+1);
		arg.typeId = (uint8_t)signature[i];
		arg.flags = 0;
		arg.len = 0;
		arg.sigPtr = NULL;
		arg.container = NULL;
		switch(signature[i]) {
			case 't':
				printf("Got a UINT64\n");
				break;
			case 'u':
				printf("Got a UINT32\n");
				break;
			case 'q':
				printf("Got a UINT16\n");
				break;
			case 'y':
				printf("Got a Byte\n");
				break;
			case 's':
				printf("Got a String %d: %s\n", i, PyString_AsString(o));
				arg.val.v_data = (void *)PyString_AsString(o);
				printf("Set data element.\n");
				break;
			case '(':
			case ')':
			case 'a':
			case 'b':
			case 'd':
			case 'g':
			case 'h':
			case 'i':
			case 'n':
			case 'o':
			case 'v':
			case 'x':
			case '{':
			case '}':
				/* Not implemented Error */
				printf("Type Not Implemented.\n");
				break;
			default:
				/* Invalid Type Error */
				printf("Invalid Type.\n");
				break;
		}
		
		printf("Marshaling arg.\n");
		_status = AJ_MarshalArg(arg1, &arg);

		if (_status != AJ_OK)
		{
			printf("Error marshaling argument: %s\n", AJ_StatusText(_status));
		} else {
			printf("Marshalled Argument!\n");
		}
	 }
	resultobj = SWIG_From_int((int)(_status));
  	return resultobj;
}