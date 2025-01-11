#include <cmath>

#include "win/gl/GLAtlas.hpp"

#ifdef WIN_USE_OPENGL

static const float gammadecode[] = {
	0.000000000000000f, 0.000303526991047f, 0.000607053982094f, 0.000910580973141f, 0.001214107964188f, 0.001517634955235f, 0.001821161946282f, 0.002124688820913f,
	0.002428215928376f, 0.002731743035838f, 0.003035269910470f, 0.003346535610035f, 0.003676506923512f, 0.004024717025459f, 0.004391442053020f, 0.004776953253895f,
	0.005181516986340f, 0.005605391692370f, 0.006048832554370f, 0.006512091029435f, 0.006995410192758f, 0.007499031722546f, 0.008023192174733f, 0.008568124845624f,
	0.009134056977928f, 0.009721217676997f, 0.010329823009670f, 0.010960093699396f, 0.011612244881690f, 0.012286487035453f, 0.012983030639589f, 0.013702080585063f,
	0.014443843625486f, 0.015208514407277f, 0.015996292233467f, 0.016807375475764f, 0.017641952261329f, 0.018500218167901f, 0.019382361322641f, 0.020288562402129f,
	0.021219009533525f, 0.022173883393407f, 0.023153364658356f, 0.024157630279660f, 0.025186857208610f, 0.026241222396493f, 0.027320891618729f, 0.028426038101315f,
	0.029556833207607f, 0.030713440850377f, 0.031896028667688f, 0.033104762434959f, 0.034339807927608f, 0.035601314157248f, 0.036889445036650f, 0.038204364478588f,
	0.039546236395836f, 0.040915198624134f, 0.042311411350965f, 0.043735027313232f, 0.045186202973127f, 0.046665083616972f, 0.048171821981668f, 0.049706563353539f,
	0.051269467920065f, 0.052860654890537f, 0.054480280727148f, 0.056128494441509f, 0.057805433869362f, 0.059511240571737f, 0.061246071010828f, 0.063010029494762f,
	0.064803279936314f, 0.066625952720642f, 0.068478181958199f, 0.070360109210014f, 0.072271861135960f, 0.074213579297066f, 0.076185390353203f, 0.078187428414822f,
	0.080219827592373f, 0.082282714545727f, 0.084376215934753f, 0.086500465869904f, 0.088655605912209f, 0.090841732919216f, 0.093058981001377f, 0.095307484269142f,
	0.097587361931801f, 0.099898740649223f, 0.102241747081280f, 0.104616492986679f, 0.107023112475872f, 0.109461717307568f, 0.111932434141636f, 0.114435382187366f,
	0.116970673203468f, 0.119538433849812f, 0.122138798236847f, 0.124771840870380f, 0.127437695860863f, 0.130136489868164f, 0.132868334650993f, 0.135633349418640f,
	0.138431623578072f, 0.141263306140900f, 0.144128486514091f, 0.147027283906937f, 0.149959802627563f, 0.152926161885262f, 0.155926465988159f, 0.158960863947868f,
	0.162029400467873f, 0.165132224559784f, 0.168269395828247f, 0.171441093087196f, 0.174647390842438f, 0.177888408303261f, 0.181164234876633f, 0.184474989771843f,
	0.187820762395859f, 0.191201671957970f, 0.194617807865143f, 0.198069304227829f, 0.201556235551834f, 0.205078706145287f, 0.208636850118637f, 0.212230727076530f,
	0.215860530734062f, 0.219526231288910f, 0.223227977752686f, 0.226965889334679f, 0.230740070343018f, 0.234550654888153f, 0.238397657871246f, 0.242281198501587f,
	0.246201395988464f, 0.250158369541168f, 0.254152178764343f, 0.258182913064957f, 0.262250721454620f, 0.266355663537979f, 0.270497858524323f, 0.274677366018295f,
	0.278894335031509f, 0.283148795366287f, 0.287440896034241f, 0.291770696640015f, 0.296138316392899f, 0.300543844699860f, 0.304987370967865f, 0.309468954801559f,
	0.313988745212555f, 0.318546831607819f, 0.323143243789673f, 0.327778130769730f, 0.332451581954956f, 0.337163656949997f, 0.341914445161819f, 0.346704095602036f,
	0.351532697677612f, 0.356400251388550f, 0.361306875944138f, 0.366252690553665f, 0.371237784624100f, 0.376262217760086f, 0.381326109170914f, 0.386429518461227f,
	0.391572564840317f, 0.396755307912827f, 0.401977866888046f, 0.407240301370621f, 0.412542700767517f, 0.417885154485703f, 0.423267751932144f, 0.428690552711487f,
	0.434153705835342f, 0.439657241106033f, 0.445201247930527f, 0.450785845518112f, 0.456411063671112f, 0.462077051401138f, 0.467783838510513f, 0.473531544208527f,
	0.479320228099823f, 0.485149979591370f, 0.491020888090134f, 0.496933043003082f, 0.502886593341827f, 0.508881449699402f, 0.514917790889740f, 0.520995676517487f,
	0.527115225791931f, 0.533276498317719f, 0.539479613304138f, 0.545724570751190f, 0.552011489868164f, 0.558340489864349f, 0.564711630344391f, 0.571124911308289f,
	0.577580511569977f, 0.584078490734100f, 0.590618908405304f, 0.597201883792877f, 0.603827416896820f, 0.610495626926422f, 0.617206633090973f, 0.623960435390472f,
	0.630757212638855f, 0.637596964836121f, 0.644479751586914f, 0.651405692100525f, 0.658374845981598f, 0.665387332439423f, 0.672443211078644f, 0.679542541503906f,
	0.686685442924500f, 0.693871915340424f, 0.701102018356323f, 0.708375930786133f, 0.715693652629852f, 0.723055243492126f, 0.730460882186890f, 0.737910568714142f,
	0.745404362678528f, 0.752942323684692f, 0.760524630546570f, 0.768151283264160f, 0.775822341442108f, 0.783537924289703f, 0.791298031806946f, 0.799102842807770f,
	0.806952357292175f, 0.814846694469452f, 0.822785854339600f, 0.830769956111908f, 0.838799118995667f, 0.846873283386230f, 0.854992687702179f, 0.863157272338867f,
	0.871367216110229f, 0.879622340202332f, 0.887923181056976f, 0.896269381046295f, 0.904661357402802f, 0.913098692893982f, 0.921582043170929f, 0.930110871791840f,
	0.938685894012451f, 0.947306573390961f, 0.955973505973816f, 0.964686274528503f, 0.973445475101471f, 0.982250571250916f, 0.991102218627930f, 1.000000000000000f
};

using namespace win::gl;

namespace win
{

GLAtlas::GLAtlas(Stream stream, Mode mode, GLenum texture_unit)
	: Atlas(std::move(stream))
{
	glActiveTexture(texture_unit);
	glBindTexture(GL_TEXTURE_2D, gltex.get());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mode == Mode::linear ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mode == Mode::linear ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	const int size = canvas_width * canvas_height * 4;
	std::unique_ptr<std::uint16_t[]> data(new std::uint16_t[size]);

	for (int i = 0; i < size; i += 4)
	{
		const float alpha = imgdata[i + 3] / 255.0f;

		data[i + 0] = std::roundf(gammadecode[imgdata[i + 0]] * alpha * std::numeric_limits<std::uint16_t>::max());
		data[i + 1] = std::roundf(gammadecode[imgdata[i + 1]] * alpha * std::numeric_limits<std::uint16_t>::max());
		data[i + 2] = std::roundf(gammadecode[imgdata[i + 2]] * alpha * std::numeric_limits<std::uint16_t>::max());
		data[i + 3] = std::roundf(alpha * std::numeric_limits<std::uint16_t>::max());
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, canvas_width, canvas_height, 0, GL_BGRA, GL_UNSIGNED_SHORT, data.get());

	// kill off the bitmap data cause we don't need it any more
	imgdata.reset(NULL);
}

GLuint GLAtlas::texture() const
{
	return gltex.get();
}

}

#endif
