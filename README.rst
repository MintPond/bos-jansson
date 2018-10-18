BosJansson README
=================

BosJansson is a C library forked from Jansson_
to enable serializing JSON data to BOS binary format.

Jansson_'s main features and design principles are:

- Simple and intuitive API and data model

- `Comprehensive documentation`_

- No dependencies on other libraries

- Full Unicode support (UTF-8)

- Extensive test suite

BosJansson is licensed under the `MIT license`_; see LICENSE in the
source distribution for details.


Compilation and Installation
----------------------------

If you obtained a source tarball, just use the standard autotools
commands::

   $ ./configure
   $ make
   $ make install

To run the test suite, invoke::

   $ make check

If the source has been checked out from a Git repository, the
./configure script has to be generated first. The easiest way is to
use autoreconf::

   $ autoreconf -i


BOS Documentation
-----------------
The binary format documentation can be found at https://github.com/MintPond/node-bos/blob/master/FORMAT.md

Note that the BOS serializer and deserializer as well as their tests assume a little endian byte ordered system.

Serialization
~~~~~~~~~~~~~
The `bos_serialize` function takes a Jansson_ `json_t*` input and
converts it into a `bson_t*` output::

    #include <bosjansson.h>

    /* serialize */

    json_error_t *error = (json_error_t *)malloc(sizeof(json_error_t));

    bos_t *serialized = bos_serialize(root, error);

    if (serialized == NULL) {
       /* There was an error during serialization */
       return;
    }

    const void *data = serialized->data;
    unsigned int size = serialized->size;

    /* ... do stuff ... */

    free(error);
    bos_free(serialized); // bos_free is provided to handle freeing bos_t type.

- The `bos_t` type should be freed using the `bos_free(bos_t *ptr)` function when finished with it.
- The serializer does not check for circular references. It is up to the user to prevent them. Circular references can result in infinite loops.
- In the event of an error a null pointer is returned and the error info is set in the provided `json_error_t` argument.

Deserialization
~~~~~~~~~~~~~~~
The `bos_deserialize` function takes a `const void *` input to the
serialized data and converts it into a `json_t*` output::

    #include <bosjansson.h>

    /* validate data */
    if (!bos_validate(serialized->data, serialized->size)) {
        /* not valid */
        return;
    }

    /* get data size */
    unsigned int data_size = bos_sizeof(serialized->data);

    /* deserialize */
    json_error_t *error = (json_error_t *)malloc(sizeof(json_error_t));

    json_t *deserialized = bos_deserialize(serialized->data, error);

    if (deserialized == NULL) {
       /* There was an error during deserialization */
       return;
    }

    /* ... do stuff ... */

    free(error);
    json_decref(deserialized);

- Use `json_decref` on the result to decrement the reference count when finished. Do not free it from memory directly.
- The size of serialized data available is determined by the first 4 bytes of the serialized data. If the data is incomplete it could lead to out of bounds memory access.
- The `bos_validate(const void *data, size_t size)` function compares the size specified by the first 4 bytes of the serialized data against the size of the allocated memory as specified in the 2nd argument. It returns true if the the size specified in the serialized data is less than or equal to the size specified in the second argument.
- The `bos_sizeof(const void *data);` function reads the first 4 bytes of the serialized data to get the size of the serialized data.
- In the event of an error a null pointer is returned and the error info is set in the provided `json_error_t` argument.

Jansson Documentation
---------------------

Documentation is available at http://jansson.readthedocs.io/en/latest/.

The documentation source is in the ``doc/`` subdirectory. To generate
HTML documentation, invoke::

   $ make html

Then, point your browser to ``doc/_build/html/index.html``. Sphinx_
1.0 or newer is required to generate the documentation.


.. _Jansson: http://www.digip.org/jansson/
.. _`Comprehensive documentation`: http://jansson.readthedocs.io/en/latest/
.. _`MIT license`: http://www.opensource.org/licenses/mit-license.php
.. _Sphinx: http://sphinx.pocoo.org/