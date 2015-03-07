{
  'targets': [
    {
      'target_name': 'alljoyn',
      'sources': [
      	'swig/alljoyn_javascript_wrap.cc',
		    'src/aj_init.c',
    	  'src/aj_about.c',
    		'src/aj_bufio.c',
    		'src/aj_bus.c',
    		'src/aj_cert.c',
    		'src/aj_connect.c',
    		'src/aj_crc16.c',
    		'src/aj_creds.c',
    		'src/aj_crypto.c',
    		'src/aj_debug.c',
    		'src/aj_disco.c',
    		'src/aj_guid.c',
    		'src/aj_init.c',
    		'src/aj_introspect.c',
    		'src/aj_keyauthentication.c',
    		'src/aj_keyexchange.c',
    		'src/aj_link_timeout.c',
    		'src/aj_msg.c',
    		'src/aj_nvram.c',
    		'src/aj_peer.c',
    		'src/aj_std.c',
    		'src/aj_util.c',
    		'src/aj_helper.c',
    		'crypto/ecc/aj_crypto_ecc.c',
    		'crypto/ecc/aj_crypto_sha2.c',
    		'external/sha2/sha2.c',
      ],
      'include_dirs': [
        'inc',
        'external/sha2'
      ],
      'conditions': [
        ['OS=="win"',
          {
            'sources': [
              'target/win32/aj_net.c',
              'target/win32/aj_target_crypto.c',
              'target/win32/aj_target_nvram.c',
              'target/win32/aj_target_util.c',
            ],
            'include_dirs': [
           	  'target/darwin'
            ],
            'msvs_settings': {
              'VCCLCompilerTool': {
                'ExceptionHandling': '2',
                'DisableSpecificWarnings': [ '4530', '4506' ],
              },
            },
          },
        ],
        ['OS=="mac"',
          {
            'sources': [
              'target/darwin/aj_net.c',
              'target/darwin/aj_target_crypto.c',
              'target/darwin/aj_target_nvram.c',
              'target/darwin/aj_target_util.c',
            ],
            'include_dirs': [
           	  'target/darwin'
            ],
            'xcode_settings': {
              'OTHER_LDFLAGS': [
                '-framework CoreFoundation -framework IOKit -framework Security'
              ]
            }
          }
        ],
        ['OS!="win"',
          {
            'sources': [
            ],
          }
        ],
      ],
    }
  ],
}
