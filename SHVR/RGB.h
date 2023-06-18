//
//  RGB.h
//
//
//  Created by Andrew Owens on 2015-02-07.
//
//

#include <iostream>

namespace Color {

	template <typename T> struct ColorRGB {
		ColorRGB(T const &r = T(), T const &g = T(), T const &b = T());

		union {
			struct {
				T r, g, b;
			};
			T rgb[3];
		};

		template <typename C> operator ColorRGB<C>() const;

		ColorRGB operator+(ColorRGB const &other) const;
		ColorRGB operator-(ColorRGB const &other) const;
		ColorRGB operator*(T const &s) const;
		ColorRGB operator/(T const &s) const;

		void operator+=(ColorRGB const &other);
		void operator-=(ColorRGB const &other);
		void operator*=(T const &s);
		void operator/=(T const &s);

		T *data();
		T const *data() const;
		T const &operator[](size_t idx) const;
		T &operator[](size_t idx);
	};

	// Material
	template <typename T> struct Material {
		typedef ColorRGB<T> Color;

		unsigned char id;
		T transparency;
		Color ambient;
		Color diffuse;
		Color emission;
		Color specular;
		T shininess;

		template <typename C> operator Material<C>() const;
	};

	template <typename T> template <typename C> ColorRGB<T>::operator ColorRGB<C>() const {
		return ColorRGB<C>(static_cast<C>(r), static_cast<C>(g), static_cast<C>(b));
	}

	template <typename T>
	template <typename C>
	Material<T>::operator Material<C>() const {
		Material<C> mat;
		mat.id = id;
		mat.transparency = static_cast<T>(transparency);
		mat.ambient = static_cast<ColorRGB<T>>(ambient);
		mat.diffuse = static_cast<ColorRGB<T>>(diffuse);
		mat.emission = static_cast<ColorRGB<T>>(emission);
		mat.specular = static_cast<ColorRGB<T>>(specular);
		mat.shininess = static_cast<T>(shininess);

		return mat;
	}

	template <typename T>
	ColorRGB<T>::ColorRGB(T const &r, T const &g, T const &b)
		: r(r), g(g), b(b) {}

	template <typename T> ColorRGB<T> ColorRGB<T>::operator+(ColorRGB<T> const &o) const {
		return ColorRGB(r + o.r, g + o.g, b + o.b);
	}

	template <typename T> ColorRGB<T> ColorRGB<T>::operator-(ColorRGB<T> const &o) const {
		return ColorRGB(r - o.r, g - o.g, b - o.b);
	}

	template <typename T> ColorRGB<T> ColorRGB<T>::operator*(T const &s) const {
		return ColorRGB(r * s, g * s, b * s);
	}

	template <typename T> ColorRGB<T> ColorRGB<T>::operator/(T const &s) const {
		return ColorRGB(r / s, g / s, b / s);
	}

	template <typename T> void ColorRGB<T>::operator+=(ColorRGB<T> const &o) {
		r += o.r;
		g += o.g;
		b += o.b;
	}

	template <typename T> void ColorRGB<T>::operator-=(ColorRGB<T> const &o) {
		r -= o.r;
		g -= o.g;
		b -= o.b;
	}

	template <typename T> void ColorRGB<T>::operator*=(T const &s) {
		r *= s;
		g *= s;
		b *= s;
	}

	template <typename T> void ColorRGB<T>::operator/=(T const &s) {
		r /= s;
		g /= s;
		b /= s;
	}

	template <typename T> T *ColorRGB<T>::data() { return rgb; }

	template <typename T> T const *ColorRGB<T>::data() const { return rgb; }

	template <typename T> T const &ColorRGB<T>::operator[](size_t idx) const {
		return rgb[idx];
	}

	template <typename T> T &ColorRGB<T>::operator[](size_t idx) { return rgb[idx]; }

	template <typename T>
	std::istream &operator >> (std::istream &in, ColorRGB<T> &color) {
		ColorRGB<T> rgb;
		in.read(reinterpret_cast<char *>(&rgb), sizeof(ColorRGB<T>));
		std::swap(color, rgb);
		return in; //>> color.r >> color.g >> color.b;
	}

	template <typename T>
	inline std::ostream &operator<<(std::ostream &out, ColorRGB<T> const &color) {
		return out << color.r << " " << color.g << " " << color.b;
	}

	template <>
	inline std::ostream &operator<<(std::ostream &out,
		ColorRGB<unsigned char> const &color) {
		return out << (unsigned)(color.r) << " " << (unsigned)(color.g) << " "
			<< (unsigned)(color.b);
	}

	template <typename T>
	std::istream &operator >> (std::istream &in, Material<T> &material) {

		//  in >> material.id >> material.transparency >> material.ambient >>
		//      material.diffuse >> material.emission >> material.specular >>
		//      material.shininess;
		Material<T> m;
		in.read(reinterpret_cast<char *>(&m), sizeof(Material<T>));
		std::swap(material, m);
		return in;
	}

	template <typename T>
	inline std::ostream &operator<<(std::ostream &out,
		Material<T> const &material) {
		return out << (unsigned)material.id << " " << material.transparency << " "
			<< material.ambient << " " << material.diffuse << " "
			<< material.emission << " " << material.specular << " "
			<< material.shininess;
	}

	template <>
	inline std::ostream &operator<<(std::ostream &out,
		Material<unsigned char> const &material) {
		return out << (unsigned)material.id << " " << (unsigned)material.transparency
			<< " " << material.ambient << " " << material.diffuse << " "
			<< material.emission << " " << material.specular << " "
			<< (unsigned)material.shininess;
	}
}
