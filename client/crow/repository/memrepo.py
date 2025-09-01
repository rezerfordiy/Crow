from crow.domain import SceneConfig as sr


class MemRepo:

    def __init__(self, entries=None):
        self._entries = entries


    def _check(self, element, key, value):
        if '__' not in key:
            key = key + '__eq'

        key, operator = key.split('__')

        if operator not in ['eq', 'lt', 'gt']:
            raise ValueError('Operator {} is not supported'.format(operator))

        operator = '__{}__'.format(operator)

        if key in ['size', 'price']:
            return getattr(element[key], operator)(int(value))
        elif key in ['latitude', 'longitude']:
            return getattr(element[key], operator)(float(value))

        return getattr(element[key], operator)(value)

    def get(self):
        return sr.SceneConfig.from_dict(self._entries)
