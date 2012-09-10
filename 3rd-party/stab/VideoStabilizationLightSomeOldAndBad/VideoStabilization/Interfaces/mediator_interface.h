#pragma once

interface MediatorInterface
{
	struct MediatorException
	{
		std::string data;
		MediatorException(std::string data_)
		{
			data = data_;
		}
	};

};
