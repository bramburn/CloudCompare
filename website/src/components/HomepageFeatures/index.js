import clsx from 'clsx';
import Heading from '@theme/Heading';
import styles from './styles.module.css';

const FeatureList = [
  {
    title: 'Octree-accelerated comparison',
    Svg: require('@site/static/img/feature-comparison.svg').default,
    description: (
      <>
        Compare two 3D point clouds (or a cloud and a mesh) using the
        octree-based{' '}
        <a
          href="https://www.cloudcompare.org/doc/wiki/index.php?title=Cloud-to-Cloud_Distance">
          Cloud-to-Cloud Distance
        </a>{' '}
        tool — designed for the difference maps you produce from a laser
        scanner and a reference mesh.
      </>
    ),
  },
  {
    title: '120+ million points on 2&nbsp;GB',
    Svg: require('@site/static/img/feature-scale.svg').default,
    description: (
      <>
        The original design target. CloudCompare handles dense terrestrial and
        mobile laser scans on modest hardware by streaming chunks through the
        octree rather than loading the whole cloud into memory.
      </>
    ),
  },
  {
    title: 'Plugin-based I/O and analysis',
    Svg: require('@site/static/img/feature-plugins.svg').default,
    description: (
      <>
        File formats (LAS, E57, PDAL, FBX, OBJ, PLY, …) and analysis tools
        (M3C2, CSF, RANSAC, Poisson) ship as <code>Standard</code>,{' '}
        <code>I/O</code>, and <code>GL</code> plugins. Drop in the ones you
        need, leave the rest off.
      </>
    ),
  },
  {
    title: 'Cross-platform desktop app',
    Svg: require('@site/static/img/feature-platforms.svg').default,
    description: (
      <>
        CloudCompare.exe on Windows, .AppImage / .deb on Linux, .app on macOS.
        Same Qt 6 UI, same code path, same plugin folder. Build scripts for
        each platform live in this docs site.
      </>
    ),
  },
];

function Feature({Svg, title, description}) {
  return (
    <div className={clsx('col col--6', styles.featureCol)}>
      <div className={styles.featureCard}>
        <div className={styles.featureSvgWrap}>
          <Svg className={styles.featureSvg} role="img" />
        </div>
        <div className={styles.featureBody}>
          <Heading as="h3">{title}</Heading>
          <p>{description}</p>
        </div>
      </div>
    </div>
  );
}

export default function HomepageFeatures() {
  return (
    <section className={styles.features}>
      <div className="container">
        <div className="row">
          {FeatureList.map((props, idx) => (
            <Feature key={idx} {...props} />
          ))}
        </div>
      </div>
    </section>
  );
}
