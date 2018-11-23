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

Bytes JSON Type
~~~~~~~~~~~~~~~
JSON doesn't support binary data types but for the sake of being able to use BOS with JANSSON, the ``JSON_BYTES`` ``json_type`` has been added. Dumping ``json_t`` values to string is not supported for values using the ``JSON_BYTES`` ``json_type``.

The following functions have been added for ``json_t`` values of the ``JSON_BYTES`` ``json_type``:

.. code-block:: c

    /*
     * Determine if a json_t value is a bytes value type
     *
     * @param json {json_t *} The json_t value to test
     *
     * @returns boolean
     */
    #define json_is_bytes(json) ((json) && json_typeof(json) == JSON_BYTES);

    /*
     * Create a new json_t bytes value.
     *
     * @param data {void *} Pointer to the binary data the json_t value will represent.
     * @param size  {size_t} The size, in bytes, of the data.
     *
     * @returns {json_t *}
     */
    json_t *json_bytes(void *data, size_t size);

    /*
     * Get a pointer to the binary data of a json_t value.
     *
     * @param json {json_t *} Pointer to the json_t value.
     *
     * @returns {void *} Pointer to the data or NULL pointer if the json_t value is not a JSON_BYTES json_type
     */
    const void *json_bytes_value(const json_t *json);

    /*
     * Get the size, in bytes, of a JSON_BYTES json_type json_t value
     *
     * @param json {json_t *} Pointer to the json_t value.
     *
     * @returns {size_t} The size of the data or 0 if the json_t value is not a JSON_BYTES json_type.
     */
    size_t json_bytes_size(const json_t *json);

    /*
     * Set the pointer and size (in bytes) of a JSON_BYTES json_type json_t value.
     *
     * @param json {json_t *} Pointer to the json_t value to set.
     * @param data {void *}   Pointer to the new data.
     * @param size {size_t}   The size of the new data, in bytes.
     */
    int json_bytes_set(json_t *json, void *data, size_t size);


Serialization
~~~~~~~~~~~~~

.. code-block:: c

    /*
     * Serialize a json_t value into BOS binary format
     *
     * @param value {json_t *}       pointer to a json_t value to serialize
     * @param error {json_error_t *} pointer to an error container so errors can be reported.
     *
     * @returns {bos_t *} pointer to a bos_t value containing a pointer to the serialized `data` and the `size`.
     */
    bos_t *bos_serialize(json_t *value, json_error_t *error);

    /*
     * Use to free bos_t value memory. Frees bos_t struct as well as the serialized data.
     *
     * @param ptr {bos_t *} pointer to the bos_t value.
     */
    void bos_free(bos_t *ptr);

Example:

.. code-block:: c

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

- The ``bos_t`` type should be freed using the ``bos_free(bos_t *ptr)`` function when finished with it. If you simply want to discard the ``bos_t`` container without freeing the ``data`` pointer, use ``free`` instead.
- The serializer does not check for circular references. It is up to the user to prevent them. Circular references can result in infinite loops.
- In the event of an error a null pointer is returned and the error info is set in the provided ``json_error_t`` argument.

Deserialization
~~~~~~~~~~~~~~~

.. code-block:: c

    /*
     * Validates bos data to ensure that the format is not corrupted.
     *
     * Use this before bos_deserialize when data to be serialized cannot be guaranteed to be correct.
     *
     * @param data {cons void *} Pointer to the serialized data
     * @param size {size_t}      The size, in bytes, of the serialized data.
     *
     * @returns {int} 0 = false, 1 = true (valid)
     */
    int bos_validate(const void *data, size_t size);

    /*
     * Read the first four bytes of the data to determine the size specified by the data.
     *
     * Do no use the value from this function in the bos_validate function since the size
     * argument in the bos_validate function is compared against the value specified in the
     * first four bytes of data. Doing so will neutralize the size check.
     *
     * @param data {const void *} Pointer to the serialized data.
     *
     * @returns {unsigned int} The size of the data as specified by the data.
     */
    unsigned int bos_sizeof(const void *data);

    /*
     * Deserialize BOS binary format data into json_t value.
     *
     * @param data {const void *}    Pointer to the serialized data.
     * @param error {json_error_t *} Pointer to error output.
     *
     * @returns {json_t *} Pointer to deserialized json_t value or NULL pointer if there was an error.
     */
    json_t *bos_deserialize(const void *data, json_error_t *error);

Example:

.. code-block:: c

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

- Use ``json_decref`` on the result to decrement the reference count when finished. Do not free it from memory directly.
- The size of serialized data available is determined by the first 4 bytes of the serialized data. If the data is incomplete it could lead to out of bounds memory access.
- The ``bos_validate(const void *data, size_t size)`` function compares the size specified by the first 4 bytes of the serialized data against the size of the allocated memory as specified in the 2nd argument. It then reads through the formatted data to determine if it stays within the size bounds it specified.
- The ``bos_sizeof(const void *data);`` function reads the first 4 bytes of the serialized data to get the size of the serialized data.
- In the event of an error in ``bos_deserialize``, a NULL pointer is returned and the error info is set in the provided ``json_error_t`` argument.

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