# -*- coding: utf-8 -*-

"""
Created on 2017-05-12

@author: freudluo
"""

import hashlib
import time
import functools
import logging as log
from google.protobuf.internal.containers import RepeatedCompositeFieldContainer
import monitor



class FieldMkr(object):
    def __init__(self, **kwargs):
        self.__dict__ = kwargs

    def __str__(self):
        return '\n'.join((('%s: %s' % item) for item in self.__dict__.items()))

    def __repr__(self):
        return repr(self.__dict__)


def get_pb_fields(pb_obj, parent_node=''):
    fields = []
    for f in pb_obj.DESCRIPTOR.fields:
        field_name = f.name
        full_field_name = field_name if parent_node == '' else '.'.join([parent_node, field_name])

        if isinstance(getattr(pb_obj, field_name), RepeatedCompositeFieldContainer):
            continue

        if not pb_obj.HasField(field_name):
            continue

        if parent_node == '' and field_name == 'gf':
            continue

        attr = getattr(pb_obj, field_name)

        if hasattr(attr, 'DESCRIPTOR'):  # nested message
            fields.extend(get_pb_fields(attr, full_field_name))
        else:
            fields.append((full_field_name, str(attr)))

    return fields


def gen_pb_checksum(pb_pbj):
    fields = get_pb_fields(pb_pbj)
    fields = sorted(fields, key=lambda i: i[0])

    s = ';'.join(('%s=%s' % i for i in fields))
    log.debug('getting checksum, str: %s', s)

    return hashlib.md5(s.encode("utf-8")).hexdigest()
